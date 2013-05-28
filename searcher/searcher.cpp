#include "searcher.h"

#include <util/bit.h>
#include <util/code.h>

#include <iostream>
#include <map>

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

void TSearcher::Search(const char* idxFile, const char* datFile, TSearchQuery query, ostream& output) {
    ifstream idxInput("index.idx");
    ifstream datInput("index.dat");
    vector<char> idxBuffer(1 << 13);
    vector<char> datBuffer(1 << 13);
    idxInput.rdbuf()->pubsetbuf(&idxBuffer[0], idxBuffer.size());
    datInput.rdbuf()->pubsetbuf(&datBuffer[0], datBuffer.size());

    TDocId filesCount;
    uint32_t compression;
    Read(idxInput, compression);
    Read(idxInput, filesCount);
    Decoder = CreateEncoder(static_cast<ECompression>(compression));
    idxInput.seekg(filesCount * sizeof(TOffset), ios_base::cur);

    TPostingList result;
    uint32_t chunkNumber;
    Chunk.Lists.resize(TRI_COUNT);
    while (Read(idxInput, chunkNumber)) {
        if (Config.Verbose)
            cerr << "Searching in chunk " << chunkNumber << " ...\n";
        Pos = idxInput.tellg();
        BindChunkToQuery(idxInput, datInput, query);
        idxInput.seekg(Pos + static_cast<istream::off_type>((TRI_COUNT + 1) * sizeof(TOffset)));
        TDocId nextDoc;
        while ((nextDoc = query->Next()) != DOCS_END)
            result.push_back(nextDoc);
        for (unordered_set<TTrigram>::const_iterator it = InQuery.begin(); it != InQuery.end(); ++it) {
            TPostingList& list = Chunk.Lists[*it];
            list.clear();
            list.shrink_to_fit();
        }
    }
    idxInput.clear();

    if (!result.empty())
        ++result[0];
    idxInput.seekg(sizeof(uint32_t) + sizeof(TDocId));
    for (size_t i = 0; i < result.size(); ++i) {
        idxInput.seekg((result[i] - 1) * sizeof(TOffset), ios_base::cur);
        TOffset offset;
        Read(idxInput, offset);
        datInput.seekg(offset);
        TOffset size;
        Read(datInput, size);
        vector<char> str(size);
        datInput.read(&str[0], size);
        str.push_back(0);
        output << &str[0] << '\n';
    }

    delete Decoder;
}

void TSearcher::BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node) {
    if (node->Tag != NODE_TERM) {
        BindChunkToQuery(idxInput, datInput, node->Left);
        BindChunkToQuery(idxInput, datInput, node->Right);
        return;
    }

    TQueryTermNode* term = dynamic_cast<TQueryTermNode*>(node);
    TTrigram tri = term->Trigram;
    InQuery.insert(tri);
    TPostingList& list = Chunk.Lists[tri];
    term->List = &list;
    term->Cur = 0;

    if (list.empty()) {
        idxInput.seekg(Pos + static_cast<istream::off_type>(tri * sizeof(TOffset)));
        TOffset offset, nextOffset;
        Read(idxInput, offset);
        Read(idxInput, nextOffset);
        if (nextOffset > offset) {
            datInput.seekg(offset);
            Decoder->Decode(datInput, list);
        }
    }
}

////////////////////////////////////////////////////////////////
// TSearcherConfig

void TSearcherConfig::SetDefault() {
    Verbose = false;
}

void TSearcherConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(Verbose, "%d");
}

} // NCodesearch

