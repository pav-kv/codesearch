#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>

using std::condition_variable;
using std::deque;
using std::mutex;
using std::string;

namespace NCodesearch {

class TFileQueue {
public:
    TFileQueue(size_t size, size_t threshold = 1);

    void Enqueue(const char* filename);
    bool Dequeue(string& to);

    void Finish();

private:
    mutex Lock;
    condition_variable NotEmptyCond;
    condition_variable NotFullCond;

    size_t Size;
    deque<string> Queue;
    size_t EnqueueThreshold;

    bool Finished;
};

} // NCodesearch

