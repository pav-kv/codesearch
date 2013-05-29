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
    ifstream idxInput(idxFile);
    ifstream datInput(datFile);
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
    TDocId lastDoc = 0;
    while (Read(idxInput, chunkNumber)) {
        if (Config.Verbose)
            cerr << "Searching in chunk " << chunkNumber << " ...\n";
        Pos = idxInput.tellg();
        BindChunkToQuery(idxInput, datInput, query);
        idxInput.seekg(Pos + static_cast<istream::off_type>((TRI_COUNT + 1) * sizeof(TOffset)));
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
    TCacheItem& item = ChunkCache[tri];
    TPostingList& list = item.List;
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
    if (!list.empty()) {
        list[0] += item.LastDoc;
        for (size_t i = 1; i < list.size(); ++i)
            list[i] += list[i - 1];
        item.LastDoc = list.back();
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

