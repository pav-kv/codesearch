#pragma once

#include <base/config.h>

#include <string>
using std::string;

namespace NCodesearch {

class TFileIndex;
class TFileQueue;

class TListerConfig : public TConfigBase {
public:
    bool Recursive;
    bool IgnoreHidden;
    bool ListDirectories;

    unsigned int MaxDepth;

public:
    TListerConfig();

    void SetDefault();
    void Print(ostream& output) const;
};

class TLister {
public:
    explicit TLister(const TListerConfig& config, TFileIndex& fileIndex, TFileQueue& fileQueue);
    void List(const string& root) const;

private:
    void List(const char* root, unsigned int depth = 0) const;

private:
    TListerConfig Config;
    TFileIndex& FileIndex;
    TFileQueue& FileQueue;
};

} // NCodesearch

