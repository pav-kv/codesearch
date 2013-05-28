#pragma once

#include <base/config.h>
#include <base/types.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace NCodesearch {

class TEncoder;

class TIndexerConfig : public TConfigBase {
public:
    bool Verbose;

    size_t ChunkSize;
    ECompression CompressionMethod;

public:
    TIndexerConfig() {
        SetDefault();
    }

    void SetDefault();
    void Print(ostream& output) const;
};

struct TIndexChunk {
    size_t Size;
    vector<TPostingList> Lists;

    TIndexChunk()
        : Size(0)
    {
    }

    void Add(TTrigram trigram, TDocId docId) {
        Lists[trigram].push_back(docId);
        ++Size;
    }
};

class TIndexer {
public:
    TIndexer(const TIndexerConfig& config);
    ~TIndexer();
    void Index(const vector<string>& files, const char* idxFile, const char* datFile);

private:
    void Index(TDocId docId, const char* filename, ostream& idxOutput, ostream& datOutput);
    void FlushChunk(ostream& idxOutput, ostream& datOutput);

private:
    TIndexerConfig Config;
    TIndexChunk Chunk;
    TOffset Offset;
    TEncoder* Encoder;
    vector<TDocId> LastDocs;
    uint32_t ChunkNumber;
};

} // NCodesearch

