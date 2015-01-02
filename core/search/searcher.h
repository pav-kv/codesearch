#pragma once

#include <base/config.h>
#include <base/types.h>

#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

class TRegexParser;

namespace NCodesearch {

class TEncoder;
class TQueryTreeNode;

class TSearcherConfig : public TConfigBase {
public:
    bool Verbose;
    bool PrintLineNumbers;
    bool ColoredOutput;
    bool JustFilter;

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
    void Search(const char* idxFile, const char* datFile, TQueryTreeNode* query, ostream& output, const char* pattern = NULL);

private:
    void BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node);
    void GrepFile(const char* filename, TRegexParser& parser, ostream& output);

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

