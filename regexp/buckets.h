#pragma once

#include <vector>

using std::vector;

namespace NCodesearch {

template <typename T>
class TBucketTable {
public:
    typedef vector<T> TBucket;
    typedef vector<TBucket> TBuckets;

public:
    TBucketTable(size_t size = 0)
        : Buckets(size)
    { /* no-op */ }

    void Resize(size_t size) {
        Buckets.resize(size);
    }

    void Push(const T& elem, size_t bucket) {
        TBucket& buck = Buckets[bucket];
        if (buck.empty())
            Used.push_back(bucket);
        buck.push_back(elem);
    }

    size_t UsedBuckets() const {
        return Used.size();
    }

    const TBuckets& GetBuckets() const {
        return Buckets;
    }

    const vector<size_t>& GetUsedBuckets() const {
        return Used;
    }

    void Clear() {
        for (size_t i = 0, size = Used.size(); i != size; ++i) {
            TBucket& bucket = Buckets[Used[i]];
            bucket.clear();
            bucket.shrink_to_fit();
        }
        Used.clear();
    }

private:
    TBuckets Buckets;
    vector<size_t> Used;
};

} // NCodesearch

