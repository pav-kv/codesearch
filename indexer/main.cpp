#include "lister.h"

#include <index/writer.h>

#include <algorithm>
#include <iostream>

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <directory> <index>\n";
        return 1;
    }

    TListerConfig listerConfig;
    listerConfig.Verbose = true;
    TLister lister(listerConfig);
    vector<string> docs;
    lister.List(string(argv[1]), docs);
    std::sort(docs.begin(), docs.end());

    TIndexWriterConfig writerConfig;
    writerConfig.Verbose = true;
    TIndexWriter writer(writerConfig);

    string indexPath = argv[2];
    string idxPath = indexPath + ".idx";
    string datPath = indexPath + ".dat";
    writer.Index(docs, idxPath.c_str(), datPath.c_str());

    return 0;
}

