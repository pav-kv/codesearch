#include "lister.h"

#include <index/writer.h>
#include <util/program_options.h>

#include <algorithm>

using namespace NCodesearch;

class TConfig : public TProgramOptions::IConfig {
public:
    unsigned NumOfPositionalOptions() const override {
        return 0;
    }

    std::string GetProgramDescription() const {
        return "Standalone codesearch indexer.\n";
    }

    void InitOptions(po::options_description& opts, po::positional_options_description&) override {
        opts.add_options()
            ("path,p", po::value<std::string>()->default_value("."), "path to a root directory")
            ("index,i", po::value<std::string>()->required(), "index path prefix")
            ;

        po::options_description listerOpts("Lister options");

        listerOpts.add_options()
            ("verbose,v", "use verbose mode")
            ("recursive,r", po::value<bool>()->default_value(true), "traverse recursively")
            ("hidden", "do not ignore hidden directories/files")
            ("max-depth", po::value<int>()->default_value(-1), "max recursion depth")
            ;

        opts.add(listerOpts);
    }

    void ProcessVariables(const po::variables_map& variables) override {
        ListerConfig.Verbose = variables.count("verbose");
        ListerConfig.Recursive = variables["recursive"].as<bool>();
        ListerConfig.IgnoreHidden = variables.count("hidden");

        IndexWriterConfig.Verbose = variables.count("verbose");

        DocsPath = variables["path"].as<std::string>();
        IndexPath = variables["index"].as<std::string>();
    }

public:
    TListerConfig ListerConfig;
    TIndexWriterConfig IndexWriterConfig;
    std::string DocsPath;
    std::string IndexPath;
};

int main(int argc, char** argv) {
    TProgramOptions options(argc, argv);
    TConfig config;
    if (!options.Parse(&config))
        return 1;

    TLister lister(config.ListerConfig);
    vector<string> docs;
    lister.List(config.DocsPath, docs);
    std::sort(docs.begin(), docs.end());

    TIndexWriter writer(config.IndexWriterConfig);

    string idxPath = config.IndexPath + ".idx";
    string datPath = config.IndexPath + ".dat";
    writer.Index(docs, idxPath.c_str(), datPath.c_str());

    return 0;
}

