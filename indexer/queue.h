#pragma once

#include <pthread.h>
#include <deque>
#include <string>

using std::deque;
using std::string;

namespace NCodesearch {

class TFileQueue {
public:
    TFileQueue(size_t size, size_t threshold = 1);

    void Enqueue(const char* filename);
    string Dequeue();

private:
    pthread_mutex_t Lock;
    pthread_cond_t NotEmptyCond;
    pthread_cond_t NotFullCond;

    size_t Size;
    deque<string> Queue;
    size_t EnqueueThreshold;
};

} // NCodesearch

