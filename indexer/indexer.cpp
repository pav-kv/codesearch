#include "indexer.h"

#include <base/types.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
inline void Write(ostream& output, T value/*, size_t& written*/) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(T));
    //written += sizeof(T);
}

template <typename T>
inline void Read(istream& input, T& value) {
    input.read(reinterpret_cast<char*>(&value), sizeof(T));
}

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TIndexer

TIndexer::TIndexer(const TIndexerConfig& config)
    : Config(config)
    , Offset(0)
{
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

    for (TDocId i = 0; i < filesCount; ++i)
        Index(i, files[i].c_str(), idxOutput, datOutput);
    if (Chunk.Size)
        FlushChunk(idxOutput, datOutput);
}

void TIndexer::Index(TDocId docId, const char* filename, ostream& idxOutput, ostream& datOutput) {
}

void TIndexer::FlushChunk(ostream& idxOutput, ostream& datOutput) {
    for (vector<TPostingList>::iterator it = Chunk.Lists.begin(); it != Chunk.Lists.end(); ++it) {
        Write(idxOutput, Offset);
        // TODO: serialize *it
        // Offset += size;
        it->clear();
        it->shrink_to_fit();
    }
    Chunk.Size = 0;
}

////////////////////////////////////////////////////////////////
// TIndexerConfig

void TIndexerConfig::SetDefault() {
    ChunkSize = 1 << 27;
    CompressionMethod = 0;  // FIXME: efficient compression
}

void TIndexerConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(ChunkSize, "%lu");
    OUTPUT_CONFIG_HEADER(CompressionMethod);
    output << CompressionMethod << '\n';  // TODO: literal repesentation
}

} // NCodesearch

