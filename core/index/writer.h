#pragma once

#include <base/config.h>
#include <base/types.h>

#include <iostream>
#include <string>
#include <vector>

namespace NCodesearch {

using std::ostream;
using std::string;
using std::vector;

class TEncoder;

class TIndexWriterConfig : public TConfigBase {
public:
    bool Verbose;

    size_t ChunkSize;
    ECompression CompressionMethod;

public:
    TIndexWriterConfig() {
        SetDefault();
    }

    void SetDefault();
    void Print(ostream& output) const;
};

struct TIndexChunk {
    uint32_t Number;
    size_t Size;
    vector<TPostingList> Lists;
    vector<TDocId> LastDocs;

    TIndexChunk(size_t lists = 0)
        : Number(0)
        , Size(0)
        , Lists(lists)
        , LastDocs(lists)
    { /* no-op */ }

    void Reset(size_t lists = 0) {
        Number = 0;
        Size = 0;
        Lists.resize(lists);
        LastDocs.resize(lists);
    }

    void Add(TTrigram trigram, TDocId docId) {
        Lists[trigram].push_back(docId);
        ++Size;
    }
};

class TIndexWriter {
public:
    TIndexWriter(const TIndexWriterConfig& config);
    ~TIndexWriter();
    void Index(const vector<string>& files, const char* idxFile, const char* datFile);

private:
    void Index(TDocId docId, const char* filename, ostream& idxOutput, ostream& datOutput);
    void FlushChunk(ostream& idxOutput, ostream& datOutput);

private:
    TIndexWriterConfig Config;
    TIndexChunk Chunk;
    TOffset DataOffset;
    TEncoder* Encoder;
};

} // NCodesearch

