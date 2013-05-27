#pragma once

#include <base/config.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace NCodesearch {

class TListerConfig : public TConfigBase {
public:
    static const unsigned INFINITE = -1;

public:
    bool Recursive;
    bool IgnoreHidden;
    bool ListDirectories;

    unsigned MaxDepth;

public:
    TListerConfig() {
        SetDefault();
    }

    void SetDefault();
    void Print(ostream& output) const;
};

class TLister {
public:
    explicit TLister(const TListerConfig& config);

    void List(const string& root, vector<string>& docs) const {
        List(root.c_str(), docs);
    }

private:
    void List(const char* root, vector<string>& docs, unsigned depth = 0) const;

private:
    TListerConfig Config;
};

} // NCodesearch

