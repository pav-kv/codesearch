#pragma once

#include "searcher.h"

#include <core/index/writer.h>
#include <util/lister.h>

#include <algorithm>
#include <atomic>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

namespace fs = boost::filesystem;

namespace NCodesearch {

class TIndexer {
public:
    TIndexer(const char* home, const TIndexWriterConfig& config)
        : Version(0)
        , IndexWriter(config)
        , StopIndexer(false)
    {
        try {
            CreateDirectories(home);
        } catch (...) {
            std::cerr << "Could not create home directory.\n";
            throw;
        }
        Restore();

        IndexerThread = std::thread(&TIndexer::RunIndexer, this);
    }

    ~TIndexer() {
        Stop();
        IndexerThread.join();
    }

    void Stop() {
        StopIndexer = true;
    }

    std::shared_ptr<TSearcher> GetSearcher() const {
        // TODO: Use atomic_load for shared_ptr when it appears in library.
        std::lock_guard<std::mutex> lock(SearcherMutex);
        return Searcher;
    }

private:
    void CreateDirectories(const char* home) {
        HomePath = home;
        if (!fs::exists(HomePath))
            fs::create_directory(HomePath);
        DataPath = HomePath / "data";
        if (!fs::exists(DataPath) || !fs::is_directory(DataPath))
            fs::create_directory(DataPath);
        IndexPath = HomePath / "index";
        if (!fs::exists(IndexPath) || !fs::is_directory(IndexPath))
            fs::create_directory(IndexPath);
    }

    void Restore() {
        std::set<uint32_t> versions;

        for (fs::directory_iterator it(IndexPath), end = fs::directory_iterator(); it != end; ++it) {
            const fs::path& path = it->path();
            if (!fs::is_regular_file(path))
                continue;
            std::string file = path.filename().string();

            std::string codesearch = "codesearch";
            if (file.length() < codesearch.length() + 1 || file.substr(0, codesearch.length()) != codesearch || file[codesearch.length()] != '.')
                continue;
            size_t pointPos = file.find_last_of('.');
            std::string ext = file.substr(pointPos + 1);
            if (ext != "idx" && ext != "dat")
                continue;
            std::string versionStr = file.substr(codesearch.length() + 1, pointPos - codesearch.length() - 1);
            if (versionStr.empty())
                continue;

            uint32_t version = atol(versionStr.c_str());
            versions.insert(version);
        }

        auto verIter = versions.begin();
        if (verIter == versions.end()) {
            Reindex(1);
        } else {
            Version = *verIter;
            for (++verIter; verIter != versions.end(); ++verIter)
                RemoveVersionIndex(*verIter);
            // TODO: Check if index is corrupted.
            const std::pair<fs::path, fs::path>& paths = GetIndexPaths(Version);
            Searcher.reset(new TSearcher(paths.first.c_str(), paths.second.c_str()));
        }
    }

    void Reindex(uint32_t newVersion) {
        const std::pair<fs::path, fs::path>& paths = GetIndexPaths(newVersion);

        TListerConfig listerConfig;
        listerConfig.Verbose = true;
        TLister lister(listerConfig);
        vector<string> docs;
        lister.List(DataPath.string(), docs);
        std::sort(docs.begin(), docs.end());

        IndexWriter.Index(docs, paths.first.c_str(), paths.second.c_str());
        std::shared_ptr<TSearcher> searcher(new TSearcher(paths.first.c_str(), paths.second.c_str()));
        {
            std::lock_guard<std::mutex> lock(SearcherMutex);
            searcher.swap(Searcher);
        }

        RemoveVersionIndex(Version);
        Version = newVersion;
    }

    std::pair<fs::path, fs::path> GetIndexPaths(uint32_t version) {
        char buffer[32];
        snprintf(buffer, 32, "codesearch.%u.", version);

        std::pair<fs::path, fs::path> result;
        result.first = IndexPath / (std::string(buffer) + "idx");
        result.second = IndexPath / (std::string(buffer) + "dat");

        return result;
    }

    void RemoveVersionIndex(uint32_t version) {
        const std::pair<fs::path, fs::path>& paths = GetIndexPaths(version);
        fs::remove(paths.first);
        fs::remove(paths.second);
    }

    void RunIndexer() {
        while (!StopIndexer) {
            sleep(1800); // 30 minutes.
            Reindex(Version + 1);
        }
    }

private:
    fs::path HomePath;
    fs::path IndexPath;
    fs::path DataPath;

    uint32_t Version;
    mutable std::mutex SearcherMutex;
    std::shared_ptr<TSearcher> Searcher;

    TIndexWriter IndexWriter;

    std::atomic_bool StopIndexer;
    std::thread IndexerThread;
};

} // NCodesearch

