#pragma once

#include <iostream>
using std::ostream;

namespace NCodesearch {

class TConfigBase {
public:
    virtual ~TConfigBase() {
    }

    virtual void SetDefault() = 0;
    virtual void Print(ostream& output) const = 0;
};

} // NCodesearch

