#include <base/types.h>
#include <core/search/searcher.h>
#include <core/query.h>
#include <util/program_options.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace NCodesearch;

class TConfig : public TProgramOptions::IConfig {
public:
    unsigned NumOfPositionalOptions() const override {
        return 1;
    }

    std::string GetProgramDescription() const {
        return "Standalone codesearch searcher.\n";
    }

    void InitOptions(po::options_description& opts, po::positional_options_description& posOpts) override {
        opts.add_options()
            ("regexp", po::value<std::string>()->required(), "query regexp")
            ("index,i", po::value<std::string>()->required(), "index path prefix")
            ("filter,f", po::value<std::string>()->default_value("++"), "trigram index filter query")
            ;

        po::options_description searcherOpts("Searcher options");
        searcherOpts.add_options()
            ("verbose,v", "use verbose mode")
            ("color", po::value<bool>()->default_value(true), "use colored output")
            ("line-number,n", "prefix matched lines with 1-base line number within a file")
            ("files-with-matches,l", "suppress normal output by printing each file matching a trigram query")
            ;

        opts.add(searcherOpts);

        posOpts.add("regexp", 1);
    }

    void ProcessVariables(const po::variables_map& variables) override {
        SearcherConfig.Verbose = variables.count("verbose");
        SearcherConfig.ColoredOutput = variables["color"].as<bool>();
        SearcherConfig.PrintLineNumbers = variables.count("line-number");
        SearcherConfig.JustFilter = variables.count("files-with-matches");

        IndexPath = variables["index"].as<std::string>();
        Regexp = variables["regexp"].as<std::string>();
        Query = variables["filter"].as<std::string>();
        if (Query == "++")
            Query = Regexp;
    }

public:
    TSearcherConfig SearcherConfig;
    std::string IndexPath;
    std::string Query;
    std::string Regexp;
};

int main(int argc, char** argv) {
    TProgramOptions options(argc, argv);
    TConfig config;
    if (!options.Parse(&config))
        return 1;

    TQueryTreeNode* query = TQueryFactory::Parse(config.Query);
    string idxPath = config.IndexPath + ".idx";
    string datPath = config.IndexPath + ".dat";
    TSearcher searcher(config.SearcherConfig);
    searcher.Search(idxPath.c_str(), datPath.c_str(), query, cout, config.Regexp.c_str());

    TQueryFactory::Free(query);

    return 0;
}

