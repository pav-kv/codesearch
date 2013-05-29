#pragma once

#include <base/types.h>
#include <algorithm>

using std::min;

namespace NCodesearch {

const int NODE_UNKNOWN = 0;
const int NODE_OR      = 1;
const int NODE_AND     = 2;
const int NODE_TERM    = 3;

const TDocId DOCS_END  = static_cast<TDocId>(-1);

struct TQueryTreeNode {
    int Tag;
    TQueryTreeNode* Left;
    TQueryTreeNode* Right;

    TQueryTreeNode(int tag = NODE_UNKNOWN, TQueryTreeNode* left = NULL, TQueryTreeNode* right = NULL)
        : Tag(tag)
        , Left(left)
        , Right(right)
    {
    }

    virtual TDocId Peek() = 0;
    virtual TDocId Next() = 0;
    virtual ~TQueryTreeNode() { }
};

struct TQueryTermNode : public TQueryTreeNode {
    TTrigram Trigram;
    const TPostingList* List;
    size_t Cur;

    TQueryTermNode(TTrigram tri, const TPostingList* list = NULL)
        : TQueryTreeNode(NODE_TERM)
        , Trigram(tri)
        , List(list)
        , Cur(0)
    {
    }

    virtual TDocId Peek() {
        return Cur < List->size() ? (*List)[Cur] : DOCS_END;
    }

    virtual TDocId Next() {
        return Cur < List->size() ? (*List)[Cur++] : DOCS_END;
    }
};

struct TQueryAndNode : public TQueryTreeNode {
    TQueryAndNode(TQueryTreeNode* left, TQueryTreeNode* right)
        : TQueryTreeNode(NODE_AND, left, right)
    {
    }

    virtual TDocId Peek() {
        while (true) {
            TDocId left = Left->Peek();
            if (left == DOCS_END)
                return DOCS_END;
            TDocId right = DOCS_END;
            while ((right = Right->Peek()) < left)
                Right->Next();
            if (right == left)
                return left;
            if (right == DOCS_END)
                return DOCS_END;
            Left->Next();
        }
    }

    virtual TDocId Next() {
        return Peek() == DOCS_END ? DOCS_END : Left->Next();
    }
};

struct TQueryOrNode : public TQueryTreeNode {
    TQueryOrNode(TQueryTreeNode* left, TQueryTreeNode* right)
        : TQueryTreeNode(NODE_OR, left, right)
    {
    }

    virtual TDocId Peek() {
        TDocId left = Left->Peek();
        TDocId right = Right->Peek();
        return min(left, right);
    }

    virtual TDocId Next() {
        TDocId left = Left->Peek();
        TDocId right = Right->Peek();
        if (left < right)
            return Left->Next();
        if (right < left)
            return Right->Next();
        if (left == DOCS_END)
            return DOCS_END;
        Left->Next();
        return Right->Next();
    }
};

typedef TQueryTreeNode* TSearchQuery;

} // NCodesearch

