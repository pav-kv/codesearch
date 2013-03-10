#include "queue.h"

#include <iostream>

using std::cerr;
using std::unique_lock;

namespace NCodesearch {

TFileQueue::TFileQueue(size_t size, size_t threshold)
    : Size(size)
    , EnqueueThreshold(threshold)
{
}

void TFileQueue::Enqueue(const char* filename) {
    unique_lock<mutex> lock(Lock);
    while (Queue.size() == Size)
        NotFullCond.wait(lock);
    bool wasEmpty = Queue.empty();
    Queue.push_back(filename);
    if (wasEmpty)
        NotEmptyCond.notify_all();
}

bool TFileQueue::Dequeue(string& to) {
    unique_lock<mutex> lock(Lock);
    if (Finished && Queue.empty())
        return false;
    while (Queue.empty())
        NotEmptyCond.wait(lock);
    bool wasFull = Queue.size() == Size;
    to = Queue.front();
    Queue.pop_front();
    if (wasFull)
        NotFullCond.notify_all();
    return true;
}

void TFileQueue::Finish() {
    unique_lock<mutex> lock(Lock);
    Finished = true;
}

} // NCodesearch

