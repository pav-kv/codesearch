#pragma once

#include <base/types.h>

#include <algorithm>
#include <iostream>

using namespace std;

template <typename T>
inline void Write(ostream& output, T value, size_t& written) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(T));
    written += sizeof(T);
}

template <typename T>
inline istream& Read(istream& input, T& value) {
    return input.read(reinterpret_cast<char*>(&value), sizeof(T));
}

namespace NCodesearch {

class TBitOutput {
public:
    TBitOutput(ostream& output)
        : Output(output)
        , Buffer(0)
        , Pos(0)
        , Written(0)
    {
    }

    ~TBitOutput() {
        if (Pos) Flush();
    }

    void WriteBit(int bit, size_t count = 1) {
        if (Pos == 8) Flush();
        while (count) {
            int cur_count = min(size_t(8) - Pos, count);
            int bits = bit ? ((1 << cur_count) - 1) << Pos : 0;
            Buffer |= TByte(bits);
            Pos += cur_count;
            count -= cur_count;
            if (Pos == 8) Flush();
        }
    }

    void WriteInt(int value, int count = 32) {
        if (Pos == 8) Flush();
        while (count) {
            int cur_count = min(8 - Pos, count);
            int bits = ((1 << cur_count) - 1) & value;
            Buffer |= bits << Pos;
            Pos += cur_count;
            count -= cur_count;
            value >>= cur_count;
            if (Pos == 8) Flush();
        }
    }

    size_t Finish() {
        if (Pos) Flush();
        return Written;
    }

    void Flush() {
        Write(Output, Buffer, Written);
        Buffer = 0;
        Pos = 0;
    }

private:
    ostream& Output;
    TByte Buffer;
    int Pos;
    size_t Written;
};

class TBitInput {
public:
    TBitInput(istream& input)
        : Input(input)
        , Buffer(0)
        , Pos(8)
    {
    }

    int ReadBit() {
        if (Pos == 8) LoadByte();
        return (Buffer >> Pos++) & 1;
    }

    int ReadInt(int count = 32) {
        int result = 0;
        int res_pos = 0;
        while (count) {
            if (Pos == 8) LoadByte();
            int cur_count = min(8 - Pos, count);
            int bits = ((1 << cur_count) - 1) & (Buffer >> Pos);
            result |= bits << res_pos;
            Pos += cur_count;
            count -= cur_count;
            res_pos += cur_count;
        }
        return result;
    }

    void Flush() {
        if (Pos) Pos = 8;
    }

    void LoadByte() {
        Read(Input, Buffer);
        Pos = 0;
    }

private:
    istream& Input;
    TByte Buffer;
    int Pos;
};

} // NCodesearch

