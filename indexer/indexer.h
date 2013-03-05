#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace NCodesearch {

class TIndexer {
public:
    TIndexer(const string& filename, size_t chunkSize);

    void Add(const string& filename);

private:
    vector<int> Trigrams;
};

} // NCodesearch

