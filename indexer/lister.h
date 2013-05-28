#pragma once

#include <base/config.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace NCodesearch {

class TListerConfig : public TConfigBase {
public:
    static const unsigned INFINITE = -1;

public:
    bool Verbose;
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
        if (Config.Verbose)
            cerr << "List: " << root << '\n';
        List(root.c_str(), docs);
    }

private:
    void List(const char* root, vector<string>& docs, unsigned depth = 0) const;

private:
    TListerConfig Config;
};

} // NCodesearch

