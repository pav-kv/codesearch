#include "searcher.h"
#include "server.h"

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <index> <proxy_host>\n";
        return 1;
    }

    std::string indexPath = argv[1];
    std::string idxPath = indexPath + ".idx";
    std::string datPath = indexPath + ".dat";
    std::string proxyHost = argv[2];

    TSearcher searcher(idxPath.c_str(), datPath.c_str());
    TCoreServer server(searcher, proxyHost.c_str());
    server.Start();

    return 0;
}

