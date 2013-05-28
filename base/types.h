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

}

