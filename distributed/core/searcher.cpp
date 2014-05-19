#include "searcher.h"

#include <base/types.h>
#include <searcher/query.h>
#include <util/code.h>
#include <util/regex.h>

#include <ctime>
#include <sstream>

namespace NCodesearch {

TSearcher::TSearcher(const char* idxFile, const char* datFile)
    : idxInput(idxFile)
    , datInput(datFile)
    , idxBuffer(1 << 13)
    , datBuffer(1 << 13)
{
    idxInput.rdbuf()->pubsetbuf(&idxBuffer[0], idxBuffer.size());
    datInput.rdbuf()->pubsetbuf(&datBuffer[0], datBuffer.size());

    TDocId filesCount;
    uint32_t compression;
    Read(idxInput, compression);
    Read(idxInput, filesCount);
    Decoder = CreateEncoder(static_cast<ECompression>(compression));

    Paths.resize(filesCount);
    for (TDocId docId = 0; docId != filesCount; ++docId) {
        // TODO: Do not make random reads.
        TOffset pathOffset;
        Read(idxInput, pathOffset);
        datInput.seekg(pathOffset);

        TOffset pathSize;
        Read(datInput, pathSize);
        vector<char> pathBuffer(pathSize);
        datInput.read(&pathBuffer[0], pathSize);
        Paths[docId].assign(pathBuffer.begin(), pathBuffer.end());
    }

    cerr << "Files in index: " << filesCount << '\n';
}

TSearcher::~TSearcher() {
    delete Decoder;
}

void TSearcher::Search(TQueryTreeNode* query, const TSearchConfig& config, ostream& output, const char* pattern) {
    if (config.Verbose) {
        cerr << "Query:\n";
        TQueryFactory::Print(query, cerr);
        cerr << "======\n";
    }

    idxInput.clear();
    datInput.clear();
    idxInput.seekg(sizeof(TDocId) + sizeof(uint32_t) + Paths.size() * sizeof(TOffset), ios_base::beg);

    clock_t timeBegin = clock();

    TPostingList result;
    uint32_t chunkNumber;
    TDocId lastDoc = 1;
    while (Read(idxInput, chunkNumber)) {
        if (config.Verbose)
            cerr << "Searching in chunk " << chunkNumber << " ...\n";
        ifstream::pos_type pos = idxInput.tellg();
        BindChunkToQuery(idxInput, datInput, query, pos);
        idxInput.seekg(pos + static_cast<istream::off_type>((TRI_COUNT + 1) * sizeof(TOffset)));
        TDocId nextDoc;
        while ((nextDoc = query->Next()) != DOCS_END) {
            result.push_back(nextDoc);
            lastDoc = nextDoc;
        }
        for (unordered_map<TTrigram, TCacheItem>::iterator it = ChunkCache.begin(), end = ChunkCache.end(); it != end; ++it) {
            TPostingList& list = it->second.List;
            list.clear();
            list.shrink_to_fit();
        }
    }
    for (unordered_map<TTrigram, TCacheItem>::iterator it = ChunkCache.begin(), end = ChunkCache.end(); it != end; ++it)
        it->second.LastDoc = 0;

    TRegexParser parser(pattern); // TODO: check compilation

    clock_t timeEnd = clock();
    cerr << "DocId time: " << double(timeEnd - timeBegin) / CLOCKS_PER_SEC << '\n';
    cerr << "Candidate documents: " << result.size() << '\n';

    for (size_t i = 0; i < result.size(); ++i) {
        const string& path = Paths[result[i] - 1];
        if (config.JustFilter)
            output << path << '\n';
        else
            GrepFile(path.c_str(), parser, output, config);
    }

    timeEnd = clock();
    cerr << "Response time: " << double(timeEnd - timeBegin) / CLOCKS_PER_SEC << '\n';
}

void TSearcher::BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node, ifstream::pos_type pos) {
    if (node->Type != NODE_TERM) {
        BindChunkToQuery(idxInput, datInput, node->Left, pos);
        BindChunkToQuery(idxInput, datInput, node->Right, pos);
        return;
    }

    TQueryTermNode* term = dynamic_cast<TQueryTermNode*>(node);
    TTrigram tri = term->Trigram;
    TCacheItem& item = ChunkCache[tri];
    TPostingList& list = item.List;
    term->List = &list;
    term->Cur = 0;

    bool newList = false;
    if (list.empty()) { // TODO: Check it when insert is called.
        idxInput.seekg(pos + static_cast<istream::off_type>(tri * sizeof(TOffset)));
        TOffset offset, nextOffset;
        Read(idxInput, offset);
        Read(idxInput, nextOffset);
        if (nextOffset > offset) {
            datInput.seekg(offset);
            Decoder->Decode(datInput, list);
            newList = true;
        }
    }
    if (newList && !list.empty()) {
        list[0] += item.LastDoc;
        for (size_t i = 1, size = list.size(); i != size; ++i)
            list[i] += list[i - 1];
        item.LastDoc = list.back();
    }
}

void TSearcher::GrepFile(const char* filename, TRegexParser& parser, ostream& output, const TSearchConfig& config) {
    if (config.Verbose)
        cerr << "Searching in file " << filename << "\n";
    ifstream input(filename);
    input.seekg(0, ifstream::end);
    ifstream::pos_type fileSize = input.tellg();
    if (fileSize > config.MaxFileSize) {
        if (config.Verbose)
            output << "Skipped by FileSize: " << filename << '\n';
        return;
    }
    input.seekg(0, ifstream::beg);

    vector<char> buffer(1 << 13); // TODO: const BUFFER_SIZSE = 1 << 13
    input.rdbuf()->pubsetbuf(&buffer[0], buffer.size());
    string line;
    size_t lineNumber = 0;
    size_t sz = 0;
    while (getline(input, line)) {
        sz += line.size();
        ++lineNumber;
        if (!parser.Match(line.c_str()))
            continue;
        if (config.ColoredOutput) {
            std::ostringstream oss;
            oss << "\033[0;36m" << filename;
            oss << "\033[0m";
            oss << ':';
            output << oss.str();
        } else {
            output << filename << ':';
        }
        if (config.PrintLineNumbers)
            if (config.ColoredOutput) {
                std::ostringstream oss;
                oss << "\033[0;32m" << lineNumber;
                oss << "\033[0m";
                oss << ':';
                output << oss.str();
            } else {
                output << lineNumber << ':';
            }
        output << line << '\n';
    }
    cerr << "Size: " << sz << "; " << filename << '\n';
}

////////////////////////////////////////////////////////////////
// TSearchConfig

void TSearchConfig::SetDefault() {
    Verbose = false;
    PrintLineNumbers = true;
    ColoredOutput = true;
    JustFilter = false;
    MaxFileSize = 2 << 20; // 2 Mb
}

void TSearchConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(Verbose, "%d");
    OUTPUT_CONFIG_VALUE(PrintLineNumbers, "%d");
    OUTPUT_CONFIG_VALUE(ColoredOutput, "%d");
    OUTPUT_CONFIG_VALUE(JustFilter, "%d");
}

} // NCodesearch

