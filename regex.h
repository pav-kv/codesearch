#pragma once

#include <memory.h>
#include <regex.h>
#include <sys/types.h>

class TRegexParser {
public:
    TRegexParser(const char* pattern, int cflags = REG_EXTENDED | REG_NOSUB)
        : ErrCode(0)
    {
        memset(ErrBuf, sizeof(ErrBuf), 0);
        ErrCode = regcomp(&Regex, pattern, cflags);
        regerror(ErrCode, &Regex, ErrBuf, sizeof(ErrBuf));
    }

    ~TRegexParser() {
        regfree(&Regex);
    }

    bool Match(const char* string, int eflags = 0) {
        regexec(&Regex, string, 0, NULL, eflags);
    }

    const char* GetError() const {
        return ErrCode ? ErrBuf : NULL;
    }

private:
    regex_t Regex;
    int ErrCode;
    char ErrBuf[256];
};

