#include "indexer.h"
#include "server.h"

using namespace NCodesearch;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <home> <proxy_host>\n";
        return 1;
    }
    std::string proxyHost = argv[2];

    TIndexWriterConfig config;
    config.Verbose = true;
    TIndexer indexer(argv[1], config);

    TCoreServer server(indexer, proxyHost.c_str());
    server.Start();

    return 0;
}

