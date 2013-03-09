#include "queue.h"

#include <iostream>
using std::cerr;

namespace NCodesearch {

TFileQueue::TFileQueue(size_t size, size_t threshold)
    : Lock(PTHREAD_MUTEX_INITIALIZER)
    , NotEmptyCond(PTHREAD_COND_INITIALIZER)
    , NotFullCond(PTHREAD_COND_INITIALIZER)
    , Size(size)
    , EnqueueThreshold(threshold)
{
}

void TFileQueue::Enqueue(const char* filename) {
    pthread_mutex_lock(&Lock);
    while (Queue.size() == Size)
        pthread_cond_wait(&NotFullCond, &Lock);
    bool wasEmpty = Queue.empty();
    Queue.push_back(filename);
    pthread_mutex_unlock(&Lock);
    if (wasEmpty)
        pthread_cond_signal(&NotEmptyCond);
}

string TFileQueue::Dequeue() {
    pthread_mutex_lock(&Lock);
    while (Queue.empty())
        pthread_cond_wait(&NotEmptyCond, &Lock);
    bool wasFull = Queue.size() == Size;
    string result = Queue.front();
    Queue.pop_front();
    pthread_mutex_unlock(&Lock);
    if (wasFull)
        pthread_cond_signal(&NotFullCond);
    return result;
}

} // NCodesearch

