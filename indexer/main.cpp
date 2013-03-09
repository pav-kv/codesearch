#include "fileindex.h"
#include "lister.h"
#include "queue.h"

#include <iostream>

using namespace NCodesearch;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory>\n";
        return 0;
    }

    TListerConfig cfg;
    TFileIndex fileIndex("index");
    TFileQueue queue(20);
    TLister lister(cfg, fileIndex, queue);
    lister.List(string(argv[1]));

    return 0;
}

