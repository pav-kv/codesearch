#include "indexer.h"
#include "lister.h"

#include <iostream>
using std::cout;

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory>\n";
        return 0;
    }

    TListerConfig listerConfig;
    TLister lister(listerConfig);
    vector<string> docs;
    lister.List(string(argv[1]), docs);

    TIndexerConfig indexerConfig;
    indexerConfig.CompressionMethod = 2;
    TIndexer indexer(indexerConfig);
    indexer.Index(docs, "index.idx", "index.dat");

    return 0;
}

