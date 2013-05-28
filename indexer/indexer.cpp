#include "indexer.h"

#include <base/types.h>
#include <util/code.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
inline void Write(ostream& output, T value/*, size_t& written*/) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(T));
    //written += sizeof(T);
}

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TIndexer

TIndexer::TIndexer(const TIndexerConfig& config)
    : Config(config)
    , Offset(0)
{
    switch (Config.CompressionMethod) {
        case 0:
            Encoder = new TSimpleEncoder();
            break;
        case 1:
            Encoder = new TeliasGammaEncoder();
            break;
        case 2:
            Encoder = new TeliasDeltaEncoder();
            break;
        case 3:
            Encoder = new TvByteEncoder();
            break;
        case 4:
            Encoder = new TpforDeltaEncoder();
            break;
    }
}

void TIndexer::Index(const vector<string>& files, const char* idxFile, const char* datFile) {
    ofstream idxOutput(idxFile);
    ofstream datOutput(datFile);
    vector<char> idxBuffer(1 << 13);
    vector<char> datBuffer(1 << 13);
    idxOutput.rdbuf()->pubsetbuf(&idxBuffer[0], idxBuffer.size());
    datOutput.rdbuf()->pubsetbuf(&datBuffer[0], datBuffer.size());

    TDocId filesCount = files.size();
    Write(idxOutput, filesCount);
    for (TDocId i = 0; i < filesCount; ++i) {
        Write(idxOutput, Offset);
        Write(datOutput, static_cast<TOffset>(files[i].size()));
        datOutput << files[i];
        Offset += sizeof(TOffset) + files[i].size();
    }

    Chunk.Lists.resize(TRI_COUNT);
    LastDocs.resize(TRI_COUNT);
    for (TDocId i = 0; i < filesCount; ++i)
        Index(i, files[i].c_str(), idxOutput, datOutput);
    if (Chunk.Size)
        FlushChunk(idxOutput, datOutput);
}

void TIndexer::Index(TDocId docId, const char* filename, ostream& idxOutput, ostream& datOutput) {
    cerr << "Indexing: " << filename << '\n';
    ifstream input(filename);
    vector<char> buffer(1 << 13);
    input.rdbuf()->pubsetbuf(&buffer[0], buffer.size());

    char chars[4];
    chars[4] = 0;
    if (!input.get(chars[0]) || !input.get(chars[1]))
        return;
    vector<bool> used(TRI_COUNT);
    while (input.get(chars[2])) {
        TTrigram tri = TByte(chars[0]) | (TByte(chars[1]) << 8) | (TByte(chars[2]) << 16);
        if (!used[tri]) {
            Chunk.Add(tri, docId);
            if (Chunk.Size >= Config.ChunkSize)
                FlushChunk(idxOutput, datOutput);
            used[tri] = true;
        }
        chars[0] = chars[1];
        chars[1] = chars[2];
    }
}

void TIndexer::FlushChunk(ostream& idxOutput, ostream& datOutput) {
    cerr << "Flush: " << Chunk.Size << '\n';
    for (TTrigram tri = 0; tri < Chunk.Lists.size(); ++tri) {
        Write(idxOutput, Offset);
        TPostingList& list = Chunk.Lists[tri];
        if (list.empty())
            continue;
        TDocId lastDoc = list.back();
        for (size_t i = list.size() - 1; i; --i)
            list[i] -= list[i - 1];
        list[0] -= LastDocs[tri];
        LastDocs[tri] = lastDoc;
        Offset += Encoder->Encode(datOutput, list);
        list.clear();
        list.shrink_to_fit();
    }
    Write(idxOutput, Offset);
    Chunk.Size = 0;
}

////////////////////////////////////////////////////////////////
// TIndexerConfig

void TIndexerConfig::SetDefault() {
    ChunkSize = 1 << 27;
    CompressionMethod = 0;
}

void TIndexerConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(ChunkSize, "%lu");
    OUTPUT_CONFIG_HEADER(CompressionMethod);
    output << CompressionMethod << '\n';  // TODO: literal repesentation
}

} // NCodesearch

