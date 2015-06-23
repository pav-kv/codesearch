#include "searcher.h"

#include <core/query.h>
#include <util/bit.h>
#include <util/code.h>
#include <util/regex.h>
#include <util/file.h>

#include <sstream>
#include <map>
/////
#include <ctime>

using namespace std;

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TSearcher

TSearcher::TSearcher(const TSearcherConfig& config)
    : Config(config)
    , Pos(0)
{
    if (Config.Verbose) {
        cerr << "Searcher config:\n";
        Config.Print(cerr);
        cerr << "================\n";
    }
}

void TSearcher::Search(const char* idxFile, const char* datFile, TSearchQuery query, ostream& output, const char* pattern) {
    if (Config.Verbose) {
        cerr << "Query:\n";
        TQueryFactory::Print(query, cerr);
        cerr << "======\n";
    }

    TBufferedFileInput idxInput(idxFile);
    TBufferedFileInput datInput(datFile);

    TDocId filesCount;
    uint32_t compression;
    Read(idxInput, compression);
    Read(idxInput, filesCount);
    Decoder = CreateEncoder(static_cast<ECompression>(compression));
    idxInput.Get().seekg(filesCount * sizeof(TOffset), ios_base::cur);

    clock_t timeBegin = clock();

    TPostingList result;
    uint32_t chunkNumber;
    TDocId lastDoc = 1;
    while (Read(idxInput, chunkNumber)) {
        if (Config.Verbose)
            cerr << "Searching in chunk " << chunkNumber << " ...\n";
        Pos = idxInput.Get().tellg();
        BindChunkToQuery(idxInput, datInput, query);
        idxInput.Get().seekg(Pos + static_cast<istream::off_type>((TRI_COUNT + 1) * sizeof(TOffset)));
        TDocId nextDoc;
        while ((nextDoc = query->Next()) != DOCS_END) {
            result.push_back(nextDoc - lastDoc);
            lastDoc = nextDoc;
        }
        for (unordered_map<TTrigram, TCacheItem>::iterator it = ChunkCache.begin(); it != ChunkCache.end(); ++it) {
            TPostingList& list = it->second.List;
            list.clear();
            list.shrink_to_fit();
        }
    }
    idxInput.Get().clear();

    TRegexParser parser(pattern); // TODO: check compilation

    clock_t timeEnd = clock();
    cerr << "DocId time: " << double(timeEnd - timeBegin) / CLOCKS_PER_SEC << '\n';

    if (!result.empty())
        ++result[0];
    idxInput.Get().seekg(sizeof(uint32_t) + sizeof(TDocId));
    for (size_t i = 0; i < result.size(); ++i) {
        idxInput.Get().seekg((result[i] - 1) * sizeof(TOffset), ios_base::cur);
        TOffset offset;
        Read(idxInput, offset);
        datInput.Get().seekg(offset);
        TOffset size;
        Read(datInput, size);
        vector<char> str(size);
        datInput.Get().read(&str[0], size);
        string filename(str.begin(), str.end());
        if (Config.JustFilter)
            output << filename << '\n';
        else
            GrepFile(filename.c_str(), parser, output);
    }

    delete Decoder;

    timeEnd = clock();
    cerr << "Response time: " << double(timeEnd - timeBegin) / CLOCKS_PER_SEC << '\n';
}

void TSearcher::BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node) {
    if (node->Type != NODE_TERM) {
        BindChunkToQuery(idxInput, datInput, node->Left);
        BindChunkToQuery(idxInput, datInput, node->Right);
        return;
    }

    TQueryTermNode* term = dynamic_cast<TQueryTermNode*>(node);
    TTrigram tri = term->Trigram;
    TCacheItem& item = ChunkCache[tri];
    TPostingList& list = item.List;
    term->List = &list;
    term->Cur = 0;

    bool newList = false;
    if (list.empty()) {
        idxInput.seekg(Pos + static_cast<istream::off_type>(tri * sizeof(TOffset)));
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
        for (size_t i = 1; i < list.size(); ++i)
            list[i] += list[i - 1];
        item.LastDoc = list.back();
    }
}

void TSearcher::GrepFile(const char* filename, TRegexParser& parser, ostream& output) {
    if (Config.Verbose)
        cerr << "Searching in file " << filename << "\n";

    TBufferedFileInput input(filename);

    string line;
    size_t lineNumber = 0;
    while (getline(input.Get(), line)) {
        ++lineNumber;
        if (!parser.Match(line.c_str()))
            continue;
        if (Config.ColoredOutput) {
            std::ostringstream oss;
            oss << "\033[0;36m" << filename;
            oss << "\033[0m";
            oss << ':';
            output << oss.str();
        } else {
            output << filename << ':';
        }
        if (Config.PrintLineNumbers) {
            if (Config.ColoredOutput) {
                std::ostringstream oss;
                oss << "\033[0;32m" << lineNumber;
                oss << "\033[0m";
                oss << ':';
                output << oss.str();
            } else {
                output << lineNumber << ':';
            }
        }
        output << line << '\n';
    }
}

////////////////////////////////////////////////////////////////
// TSearcherConfig

void TSearcherConfig::SetDefault() {
    Verbose = false;
    PrintLineNumbers = true;
    ColoredOutput = true;
    JustFilter = false;
}

void TSearcherConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(Verbose, "%d");
    OUTPUT_CONFIG_VALUE(PrintLineNumbers, "%d");
    OUTPUT_CONFIG_VALUE(ColoredOutput, "%d");
    OUTPUT_CONFIG_VALUE(JustFilter, "%d");
}

} // NCodesearch

