#pragma once

#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>
#include <memory>

namespace NCodesearch {

namespace po = boost::program_options;

class TProgramOptions {
public:
    class IConfig;

public:
    TProgramOptions(int argc, const char* const* argv)
        : argc(argc)
        , argv(argv)
    { /* no-op */ }

    bool Parse(IConfig* config);

private:
    void PrintUsage(std::ostream& output, IConfig* config, const po::options_description& opts,
            const po::positional_options_description& posOpts);

private:
    const int argc;
    const char* const* argv;
};

class TProgramOptions::IConfig {
public:
    virtual ~IConfig() { /* no-op */ }

    virtual unsigned NumOfPositionalOptions() const = 0;
    virtual std::string GetProgramDescription() const = 0;
    virtual void InitOptions(po::options_description& opts, po::positional_options_description& posOpts) = 0;
    virtual void ProcessVariables(const po::variables_map& variables) = 0;
};

////////////////////////////////////////////////////////////////////////////////

void TProgramOptions::PrintUsage(std::ostream& output, IConfig* config,
        const po::options_description& opts, const po::positional_options_description& posOpts)
{
    output << "Usage: " << argv[0];

    if (!opts.options().empty())
        output << " [options]";
    if (posOpts.max_total_count() > 0) {
        unsigned count = std::min(posOpts.max_total_count(), config->NumOfPositionalOptions());
        for (unsigned index = 0; index != count; ++index)
            output << ' ' << posOpts.name_for_position(index);
        if (posOpts.max_total_count() > count)
            output << " ...";
    }
    output << '\n';
    output << opts << std::endl;
}

bool TProgramOptions::Parse(IConfig* config) {
    po::options_description opts("General options");
    opts.add_options()("help,?", "show help message");
    po::positional_options_description posOpts;
    config->InitOptions(opts, posOpts);

    po::variables_map varMap;
    try {
        po::store(po::command_line_parser(argc, argv).options(opts).positional(posOpts).run(), varMap);
        if (varMap.count("help")) {
            std::cout << config->GetProgramDescription() << '\n';
            PrintUsage(std::cout, config, opts, posOpts);
            return false;
        }
        po::notify(varMap);
    } catch (po::error& err) {
        std::cerr << "ERROR: " << err.what() << "\n\n";
        PrintUsage(std::cerr, config, opts, posOpts);
        return false;
    }

    config->ProcessVariables(varMap);

    return true;
}

} // namespace NCodesearch

