#include "fileindex.h"

#include <iostream>
using std::cerr;

namespace NCodesearch {

TFileIndex::TFileIndex(const char* filename)
    : Output(filename)
    , NextId(0)
{
}

TFileId TFileIndex::Insert(const char* filename) {
    std::cerr << filename << '\n';
    Output << filename << '\n';
    return NextId++;
}

} // NCodesearch

