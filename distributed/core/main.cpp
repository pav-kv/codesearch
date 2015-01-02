#include "indexer.h"
#include "server.h"

#include <util/program_options.h>

using namespace NCodesearch;

class TConfig : public TProgramOptions::IConfig {
public:
    unsigned NumOfPositionalOptions() const override {
        return 0;
    }

    std::string GetProgramDescription() const {
        return "codesearch core.\n";
    }

    void InitOptions(po::options_description& opts, po::positional_options_description&) override {
        opts.add_options()
            ("home", po::value<std::string>()->required(), "home directory")
            ("proxy,p", po::value<std::string>()->default_value("localhost"), "proxy host")
            ;

        po::options_description indexerOpts("Lister options");

        indexerOpts.add_options()
            ("verbose,v", "use verbose mode")
            ("chunk-size", po::value<size_t>(), "custom chunk size")
            ;

        opts.add(indexerOpts);
    }

    void ProcessVariables(const po::variables_map& variables) override {
        IndexWriterConfig.Verbose = variables.count("verbose");
        if (variables.count("chunk-size"))
            IndexWriterConfig.ChunkSize = variables["chunk-size"].as<size_t>();

        Home = variables["home"].as<std::string>();
        Proxy = variables["proxy"].as<std::string>();
    }

public:
    TIndexWriterConfig IndexWriterConfig;

    std::string Home;
    std::string Proxy;
};

int main(int argc, char** argv) {
    TProgramOptions options(argc, argv);
    TConfig config;
    if (!options.Parse(&config))
        return 1;

    TIndexer indexer(config.Home.c_str(), config.IndexWriterConfig);
    TCoreServer server(indexer, config.Proxy.c_str());
    server.Start();

    return 0;
}

