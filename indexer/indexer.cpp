#include "indexer.h"
#include "queue.h"

#include <fstream>
#include <iostream>
#include <vector>

using std::ifstream;
using std::vector;

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TIndexerWorker

void TIndexerWorker::IndexFile(const char* filename) {
    // TODO filter text files
    ifstream input(filename);
    vector<char> buffer(4096);
    input.rdbuf()->pubsetbuf(&buffer[0], buffer.size());

    char last[3]; // TODO Ngram
    input.get(last[0]);
    input.get(last[1]);
    while (input.get(last[2])) {
        unsigned trigram = (last[0] << 16) | (last[1] << 8) | last[2];
        std::cerr << "3: " << trigram << '\n';
        last[0] = last[1];
        last[1] = last[2];
    }
}

////////////////////////////////////////////////////////////////
// TIndexer

//TIndexer::TIndexer() {
//}

} // NCodesearch

