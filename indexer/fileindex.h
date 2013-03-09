#pragma once

#include <base/types.h>

#include <fstream>
#include <string>

using std::ofstream;
using std::string;

namespace NCodesearch {

class TFileIndex {
public:
    explicit TFileIndex(const char* filename);

    TFileId Insert(const char* filename);

private:
    TFileId NextId;
    ofstream Output;
};

} // NCodesearch

