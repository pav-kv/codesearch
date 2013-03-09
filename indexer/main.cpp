#include "lister.h"

#include <iostream>

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory>\n";
        return 0;
    }

    TListerConfig cfg;
    TLister lister(cfg);
    lister.List(string(argv[1]));

    return 0;
}

