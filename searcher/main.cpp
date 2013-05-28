#include "searcher.h"
#include "query.h"

#include <base/types.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace NCodesearch;

TSearchQuery GetQuery(const string& query, size_t pos = 0) {
    TTrigram tri = TByte(query[pos]) | (TByte(query[pos + 1]) << 8) | (TByte(query[pos + 2]) << 16);
    TQueryTreeNode* term = new TQueryTermNode(tri);
    if (pos + 3 < query.size()) {
        TQueryTreeNode* right = GetQuery(query, pos + 1);
        TQueryTreeNode* root = new TQueryAndNode(term, right);
        return root;
    }
    return term;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <index> <query>\n";
        return 1;
    }

    TSearchQuery query = GetQuery(argv[2]);

    string indexPath = argv[1];
    string idxPath = indexPath + ".idx";
    string datPath = indexPath + ".dat";
    TSearcherConfig config;
    config.Verbose = true;
    TSearcher searcher(config);
    searcher.Search(idxPath.c_str(), datPath.c_str(), query, cout);

    return 0;
}

