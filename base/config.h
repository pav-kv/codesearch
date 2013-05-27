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

#define OUTPUT_CONFIG_HEADER(name) \
    snprintf(buffer, sizeof(buffer), "%*s : ", 20, #name);

#define OUTPUT_CONFIG_VALUE(name, specifier) \
    snprintf(buffer, sizeof(buffer), "%*s : " specifier, 20, #name, name); \
    output << buffer << '\n';

} // NCodesearch

