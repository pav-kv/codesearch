#pragma once

namespace NCodesearch {

class TFileQueue;

class TIndexerWorker {
public:
    TIndexerWorker(TFileQueue& fileQueue);

private:
    void IndexFile(const char* filename);

private:
    // local index
};

class TIndexer {
public:
    friend class TIndexerWorker;

    TIndexer(TFileQueue& fileQueue, unsigned workers, const char* filename);

private:
};

} // NCodesearch

