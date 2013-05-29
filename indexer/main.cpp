#include "indexer.h"
#include "lister.h"

#include <iostream>
using std::cout;

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

    TIndexerConfig indexerConfig;
    indexerConfig.Verbose = true;
    TIndexer indexer(indexerConfig);

    string indexPath = argv[2];
    string idxPath = indexPath + ".idx";
    string datPath = indexPath + ".dat";
    indexer.Index(docs, idxPath.c_str(), datPath.c_str());

    return 0;
}

