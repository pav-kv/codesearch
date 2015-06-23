#pragma once

#include <fstream>
#include <vector>

template <typename TFileStream>
class TBufferedFileStreamImpl {
public:
    TBufferedFileStreamImpl(const char* fileName, size_t bufSize = 1 << 13)
        : Buffer(bufSize)
        , Stream(fileName)
    {
        Stream.rdbuf()->pubsetbuf(Buffer.data(), Buffer.size());
    }

    inline operator TFileStream&() { return Stream; }
    inline TFileStream& Get() { return Stream; }

private:
    std::vector<char> Buffer;
    TFileStream Stream;
};

using TBufferedFileInput = TBufferedFileStreamImpl<std::ifstream>;
using TBufferedFileOutput = TBufferedFileStreamImpl<std::ofstream>;

