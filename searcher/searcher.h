#pragma once

#include "query.h"

#include <base/config.h>
#include <indexer/indexer.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

namespace NCodesearch {

class TSearcherConfig : public TConfigBase {
public:
    bool Verbose;

public:
    TSearcherConfig() {
        SetDefault();
    }

    void SetDefault();
    void Print(ostream& output) const;
};

class TSearcher {
public:
    TSearcher(const TSearcherConfig& config);
    void Search(const char* idxFile, const char* datFile, TSearchQuery query, ostream& output);

private:
    void BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node);

private:
    TSearcherConfig Config;
    TEncoder* Decoder;
    ifstream::pos_type Pos;

    struct TCacheItem {
        TDocId LastDoc;
        TPostingList List;
    };
    unordered_map<TTrigram, TCacheItem> ChunkCache;
};

} // NCodesearch

