#pragma once

#include <base/types.h>
#include <base/config.h>
#include <util/regex.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

namespace NCodesearch {

class TEncoder;
class TQueryTreeNode;

class TSearchConfig : public TConfigBase {
public:
    bool Verbose;
    bool PrintLineNumbers;
    bool ColoredOutput;
    bool JustFilter;
    uint64_t MaxFileSize;

public:
    TSearchConfig() {
        SetDefault();
    }

    void SetDefault();
    void Print(ostream& output) const;
};

class TSearcher {
public:
    TSearcher(const char* idxFile, const char* datFile);
    ~TSearcher();

    void Search(TQueryTreeNode* query, const TSearchConfig& config, ostream& output, const char* pattern = NULL);

private:
    void BindChunkToQuery(ifstream& idxInput, ifstream& datInput, TQueryTreeNode* node, ifstream::pos_type pos);
    void GrepFile(const char* filename, TRegexParser& parser, ostream& output, const TSearchConfig& config);

private:
    // TODO: code style
    // TODO: make buffered input type
    ifstream idxInput;
    ifstream datInput;
    vector<char> idxBuffer;
    vector<char> datBuffer;

    std::vector<std::string> Paths;
    TEncoder* Decoder; // TODO: Smart pointer.

    struct TCacheItem {
        TDocId LastDoc;
        TPostingList List;

        TCacheItem()
            : LastDoc(0)
        { /* no-op */ }
    };
    unordered_map<TTrigram, TCacheItem> ChunkCache; // TODO: Make thread-specific on concurrent access.
};

} // NCodesearch

