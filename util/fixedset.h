#include <algorithm>
#include <vector>

template <typename TChar>
class TFixedSet {
public:
    typedef std::pair<TChar, TChar> TRange;
    typedef std::vector<TRange> TRanges;
    typedef TFixedSet<TChar> TSelf;

public:
    TFixedSet(const TRanges& ranges, bool negation)
        : Ranges(ranges)
        , Negation(negation)
    { /* no-op */ }

    bool IsNegation() const {
        return Negation;
    }

    const TRanges& GetRanges() const {
        return Ranges;
    }

    bool Contains(TChar ch) const {
        if (Ranges.empty() || ch < Ranges.front().first)
            return !Negation;
        size_t left = 0, right = Ranges.size();
        while (left + 1 < right) {
            size_t median = left + ((right - left) >> 1);
            if (ch >= Ranges[median].first)
                left = median;
            else
                right = median;
        }
        return (ch < Ranges[left].second) != Negation;
    }

    TSelf Intersect(const TSelf& rhs) const {
        if (Negation) {
            if (rhs.Negation) {
                return TSelf(Union(Ranges, rhs.Ranges), true);
            } else {
            }
        } else {
            if (rhs.Negation) {
            } else {
                return TSelf(Intersection(Ranges, rhs.Ranges), false);
            }
        }
    }

    TSelf Union(const TSelf& rhs) const {
        if (Negation) {
            if (rhs.Negation) {
                return TSelf(Intersection(Ranges, rhs.Ranges), true);
            } else {
            }
        } else {
            if (rhs.Negation) {
            } else {
                return TSelf(Union(Ranges, rhs.Ranges), false);
            }
        }
    }

    TSelf Subtract(const TSelf& rhs) const {
    }

    TSelf Negate() const {
        return TSelf(Ranges, !Negation);
    }

private:
    static TRanges Intersection(const TRanges& lhs, const TRanges& rhs);
    static TRanges Union(const TRanges& lhs, const TRanges& rhs);
    static TRanges Difference(const TRanges& lhs, const TRanges& rhs);

private:
    // TODO: counted ptr on vector
    const TRanges Ranges;
    const bool Negation;
};

template <typename TChar>
class TFixedSetBuilder {
private:
    typedef typename TFixedSet<TChar>::TRange TRange;
    typedef typename TFixedSet<TChar>::TRanges TRanges;

public:
    TFixedSetBuilder()
        : Negation(false)
    { /* no-op */ }

    void Clear() {
        Ranges.clear();
        Ranges.shrink_to_fit();
        Negation = false;
    }

    void AddRange(TChar begin, TChar end) {
        if (begin < end)
            Ranges.push_back(TRange(begin, end));
    }

    void AddChar(TChar ch) {
        Ranges.push_back(TRange(ch, ch + 1));
    }

    void SetNegation(bool negation = true) {
        Negation = negation;
    }

    TFixedSet<TChar> GetFixedSet() {
        std::sort(Ranges.begin(), Ranges.end());
        TRanges newRanges;
        TRange range = Ranges[0];
        for (size_t i = 1, size = Ranges.size(); i < size; ++i) {
            if (Ranges[i].first > range.second) {
                newRanges.push_back(range);
                range = Ranges[i];
            } else {
                range.second = Ranges[i].second;
            }
        }
        newRanges.push_back(range);
        return TFixedSet<TChar>(newRanges, Negation);
    }

private:
    TRanges Ranges;
    bool Negation;
};


template <typename TChar>
typename TFixedSet<TChar>::TRanges TFixedSet<TChar>::Intersection(const TRanges& lhs, const TRanges& rhs) {
    TRanges result;
    for (size_t i = 0, j = 0, lSize = lhs.size(), rSize = rhs.size(); i < lSize && j < rSize; ++i) {
        while (j < rSize && rhs[j].second <= lhs[i].first)
            ++j;
        for (; j < rSize && rhs[j].first < lhs[i].second; ++j)
            result.push_back(TRange(std::max(lhs[i].first, rhs[j].first), std::min(lhs[i].second, rhs[j].second)));
    }
    return result;
}

template <typename TChar>
typename TFixedSet<TChar>::TRanges TFixedSet<TChar>::Union(const TRanges& lhs, const TRanges& rhs) {
    TRanges result;
    /*for (size_t i = 0, j = 0, lSize = lhs.size(), rSize = rhs.size(); i < lSize || j < rSize; ++i) {
        while (j < rSize && rhs[j].second <= lhs[i].first)
            ++j;
        for (; j < rSize && rhs[j].first < lhs[i].second; ++j)
            result.push_back(TRange(std::max(lhs[i].first, rhs[j].first), std::min(lhs[i].second, rhs[j].second)));
    }*/
    return result;
}


template <typename TChar>
typename TFixedSet<TChar>::TRanges TFixedSet<TChar>::Difference(const TRanges& lhs, const TRanges& rhs) {
    TRanges result;
    /*for (size_t i = 0, j = 0, lSize = Ranges.size(), rSize = rhs.size(); i < lSize; ++i) {
        for (; j < rSize && rhs[j].second <= Ranges[i].first; ++j) * no-op *;
        for (; j < rSize && rhs[j].first < Ranges[i].second; ++j)
            result.push_back(TRange(std::max(Ranges[i].first, rhs[j].first), std::min(Ranges[i].second, rhs[j].second)));
    }*/
    return result;
}
