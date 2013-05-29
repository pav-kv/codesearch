#include "searcher.h"
#include "query.h"

#include <base/types.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <index> <query> <regex>\n";
        return 1;
    }

    TQueryTreeNode* query = TQueryFactory::Parse(argv[2]);
    string indexPath = argv[1];
    string idxPath = indexPath + ".idx";
    string datPath = indexPath + ".dat";
    TSearcherConfig config;
    TSearcher searcher(config);
    searcher.Search(idxPath.c_str(), datPath.c_str(), query, cout, argv[3]);

    TQueryFactory::Free(query);

    return 0;
}

