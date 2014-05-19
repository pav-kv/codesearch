#include "searcher.h"
#include "server.h"

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <index>\n";
        return 1;
    }

    std::string indexPath = argv[1];
    std::string idxPath = indexPath + ".idx";
    std::string datPath = indexPath + ".dat";

    TSearcher searcher(idxPath.c_str(), datPath.c_str());
    TCoreServer server(searcher);
    server.Start();

    return 0;
}

