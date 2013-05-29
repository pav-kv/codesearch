#pragma once

#include <memory.h>
#include <regex.h>
#include <sys/types.h>

#include <vector>

using std::vector;

class TRegexParser {
public:
    TRegexParser(const char* pattern, int cflags = REG_EXTENDED | REG_NOSUB)
        : ErrCode(0)
        , ErrBuf(256)
    {
        ErrCode = regcomp(&Regex, pattern, cflags);
        regerror(ErrCode, &Regex, &ErrBuf[0], ErrBuf.size());
    }

    ~TRegexParser() {
        regfree(&Regex);
    }

    bool Match(const char* string, int eflags = 0) {
        return !regexec(&Regex, string, 0, NULL, eflags);
    }

    const char* GetError() const {
        return ErrCode ? &ErrBuf[0] : NULL;
    }

private:
    regex_t Regex;
    int ErrCode;
    vector<char> ErrBuf;
};

