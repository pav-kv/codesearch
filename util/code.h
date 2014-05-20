#pragma once

#include "bit.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace NCodesearch {

inline int Log2(TDocId x) {
    int result = 0;
    if (x & 0xFFFF0000) {
        result += 16;
        x >>= 16;
    }
    if (x & 0xFF00) {
        result += 8;
        x >>= 8;
    }
    if (x & 0xF0) {
        result += 4;
        x >>= 4;
    }
    if (x & 0xC) {
        result += 2;
        x >>= 2;
    }
    if (x & 0x2)
        ++result;

    return result;
}

class TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const = 0;
    virtual void Decode(istream& input, TPostingList& list) const = 0;
    virtual ~TEncoder() { }
};

class TSimpleEncoder : public TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const {
        size_t written = 0;
        Write(output, list.size(), written);
        for (size_t i = 0; i < list.size(); ++i)
            Write(output, list[i], written);
        return written;
    }

    virtual void Decode(istream& input, TPostingList& list) const {
        size_t size = 0;
        Read(input, size);
        list.resize(size);
        for (size_t i = 0; i < size; ++i)
            Read(input, list[i]);
    }
};

class TeliasGammaEncoder : public TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const {
        size_t written = 0;
        TBitOutput bitOutput(output);
        Write(output, list.size(), written);
        for (size_t i = 0; i < list.size(); ++i) {
            int len = Log2(list[i]);
            bitOutput.WriteBit(1, len);
            bitOutput.WriteBit(0);
            bitOutput.WriteInt(list[i], len);
        }
        return written += bitOutput.Finish();
    }

    virtual void Decode(istream& input, TPostingList& list) const {
        size_t size = 0;
        Read(input, size);
        list.resize(size);
        TBitInput bitInput(input);
        for (size_t i = 0; i < size; ++i) {
            int len = 0;
            while (bitInput.ReadBit())
                ++len;
            list[i] = (1 << len) + bitInput.ReadInt(len);
        }
    }
};

class TeliasDeltaEncoder : public TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const {
        size_t written = 0;
        TBitOutput bitOutput(output);
        Write(output, list.size(), written);
        for (size_t i = 0; i < list.size(); ++i) {
            int len = Log2(list[i]);
            int lenlen = Log2(len + 1);
            bitOutput.WriteBit(1, lenlen);
            bitOutput.WriteBit(0);
            bitOutput.WriteInt(len + 1, lenlen);
            bitOutput.WriteInt(list[i], len);
        }
        return written += bitOutput.Finish();
    }

    virtual void Decode(istream& input, TPostingList& list) const {
        size_t size = 0;
        Read(input, size);
        list.resize(size);
        TBitInput bitInput(input);
        for (size_t i = 0; i < size; ++i) {
            int lenlen = 0;
            while (bitInput.ReadBit())
                ++lenlen;
            int len = (1 << lenlen) + bitInput.ReadInt(lenlen) - 1;
            list[i] = (1 << len) + bitInput.ReadInt(len);
        }
    }
};

class TvByteEncoder : public TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const {
        size_t written = 0;
        Write(output, list.size(), written);
        for (size_t i = 0; i < list.size(); ++i) {
            TDocId id = list[i];
            do {
                TByte byte = id & 0x7F;
                if (id >>= 7) byte |= 0x80;
                Write(output, byte, written);
            } while (id);
        }
        return written;
    }

    virtual void Decode(istream& input, TPostingList& list) const {
        size_t size = 0;
        Read(input, size);
        list.resize(size);
        for (size_t i = 0; i < size; ++i) {
            TDocId docId = 0;
            TByte byte = 0x80;
            for (int offset = 0; byte & 0x80; offset += 7) {
                Read(input, byte);
                docId |= (TDocId(byte & 0x7F) << offset);
            }
            list[i] = docId;
        }
    }
};

class TpforDeltaEncoder : public TEncoder {
public:
    virtual size_t Encode(ostream& output, const TPostingList& list) const {
        size_t written = 0;
        TBitOutput bitOutput(output);
        Write(output, list.size(), written);

        TPostingList sortedList(list);
        sort(sortedList.begin(), sortedList.end());
        TDocId threshold = sortedList[sortedList.size() * 9 / 10];
        TByte len = Log2(threshold) + 1;
        Write(output, len, written);
        TPostingList back;
        for (size_t i = 0; i < list.size(); ++i) {
            TDocId value = list[i];
            if (value > threshold) {
                back.push_back(value);
                value = 0;
            }
            bitOutput.WriteInt((int)value, len);
        }
        written += bitOutput.Finish();
        for (size_t i = 0; i < back.size(); ++i)
            Write(output, back[i], written);

        return written;
    }

    virtual void Decode(istream& input, TPostingList& list) const {
        size_t size = 0;
        Read(input, size);
        list.resize(size);
        vector<size_t> zeros;
        TByte len = 0;
        Read(input, len);
        TBitInput bitInput(input);
        for (size_t i = 0; i < size; ++i) {
            list[i] = bitInput.ReadInt(len);
            if (!list[i])
                zeros.push_back(i);
        }
        bitInput.Flush();
        for (size_t i = 0; i < zeros.size(); ++i)
            Read(input, list[zeros[i]]);
    }
};

inline TEncoder* CreateEncoder(ECompression compressionMethod) {
    switch (compressionMethod) {
    case C_NONE:
        return new TSimpleEncoder();
    case C_ELIAS_GAMMA:
        return new TeliasGammaEncoder();
    case C_ELIAS_DELTA:
        return new TeliasDeltaEncoder();
    case C_VBYTE:
        return new TvByteEncoder();
    case C_PFOR_DELTA:
        return new TpforDeltaEncoder();
    }
    return NULL;
}

} // NCodesearch

