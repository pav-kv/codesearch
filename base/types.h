#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace NCodesearch {

typedef unsigned char TByte;
typedef uint32_t TDocId;
typedef uint32_t TOffset;
typedef uint32_t TTrigram;

typedef std::vector<TDocId> TPostingList;

const size_t TRI_COUNT = 1 << 24;

enum ECompression {
    C_NONE,
    C_ELIAS_GAMMA,
    C_ELIAS_DELTA,
    C_VBYTE,
    C_PFOR_DELTA,
};

}

