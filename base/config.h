#pragma once

#include <iostream>

namespace NCodesearch {

class TConfigBase {
public:
    virtual ~TConfigBase() {
    }

    virtual void SetDefault() = 0;
    virtual void Print(std::ostream& output) const = 0;
};

} // NCodesearch

