#pragma once

#include <base/types.h>
#include <algorithm>

using std::min;

namespace NCodesearch {

enum EQueryNodeType {
    NODE_UNKNOWN = 0,
    NODE_OR      = 1,
    NODE_AND     = 2,
    NODE_TERM    = 3
};

const TDocId DOCS_END  = static_cast<TDocId>(-1);

class TQueryFactory;

class TQueryTreeNode {
public:
    const EQueryNodeType Type;

    TQueryTreeNode* Left;
    TQueryTreeNode* Right;

public:
    TQueryTreeNode(EQueryNodeType type = NODE_UNKNOWN, TQueryTreeNode* left = NULL, TQueryTreeNode* right = NULL)
        : Type(type)
        , Left(left)
        , Right(right)
        , Cache(DOCS_END)
        , UpToDate(false)
    { /* no-op */ }

    // TODO: smart pointers
    virtual ~TQueryTreeNode() {
        if (Left)
            delete Left;
        if (Right)
            delete Right;
    }

    TDocId Peek() {
        return DoPeek();
        if (!UpToDate) {
            Cache = DoPeek();
            UpToDate = true;
        }
        return Cache;
    }

    TDocId Next() {
        if (UpToDate && Cache == DOCS_END)
            return DOCS_END;
        TDocId next = DoNext();
        UpToDate = false;
        return next;
    }

private:
    virtual TDocId DoPeek() = 0;
    virtual TDocId DoNext() = 0;

protected:
    TDocId Cache;
    bool UpToDate;
};

class TQueryTermNode : public TQueryTreeNode {
public:
    const TTrigram Trigram;
    const TPostingList* List;
    size_t Cur;

public:
    TQueryTermNode(TTrigram tri, const TPostingList* list = NULL)
        : TQueryTreeNode(NODE_TERM)
        , Trigram(tri)
        , List(list)
        , Cur(0)
    { /* no-op */ }

private:
    virtual TDocId DoPeek() {
        return Cur < List->size() ? (*List)[Cur] : DOCS_END;
    }

    virtual TDocId DoNext() {
        return Cur < List->size() ? (*List)[Cur++] : DOCS_END;
    }
};

class TQueryAndNode : public TQueryTreeNode {
public:
    TQueryAndNode(TQueryTreeNode* left, TQueryTreeNode* right)
        : TQueryTreeNode(NODE_AND, left, right)
    { /* no-op */ }

private:
    virtual TDocId DoPeek() {
        for (TDocId left = Left->Peek(), right = Right->Peek(); left != DOCS_END && right != DOCS_END; ) {
            if (left == right)
                return left;
            if (left < right) {
                Left->Next();
                left = Left->Peek();
            } else {
                Right->Next();
                right = Right->Peek();
            }
        }
        return DOCS_END;
    }

    virtual TDocId DoNext() {
        return Peek() == DOCS_END ? DOCS_END : Left->Next();
    }
};

class TQueryOrNode : public TQueryTreeNode {
public:
    TQueryOrNode(TQueryTreeNode* left, TQueryTreeNode* right)
        : TQueryTreeNode(NODE_OR, left, right)
    { /* no-op */ }

private:
    virtual TDocId DoPeek() {
        TDocId left = Left->Peek();
        TDocId right = Right->Peek();
        return min(left, right);
    }

    virtual TDocId DoNext() {
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

class TQueryFactory {
public:
    static TQueryTreeNode* Parse(string query) {
        query.push_back('\0');
        size_t pos = 0;
        return Parse(query.c_str(), pos);
    }

    static void Print(const TQueryTreeNode* node, ostream& output, int depth = 0) {
        for (int i = 0; i < depth; ++i)
            output << ' ';
        switch (node->Type) {
        case NODE_OR:
            output << "OR\n";
            Print(node->Left, output, depth + 2);
            Print(node->Right, output, depth + 2);
            break;
        case NODE_AND:
            output << "AND\n";
            Print(node->Left, output, depth + 2);
            Print(node->Right, output, depth + 2);
            break;
        case NODE_TERM:
            output << "TRI: " << dynamic_cast<const TQueryTermNode*>(node)->Trigram << '\n';
            break;
        }
    }

    static void Free(TSearchQuery& query) {
        if (!query) return;
        delete query;
        query = NULL;
    }

private:
    static size_t GetControl(const char* query) {
        size_t ret = 0;
        for (const char* cur = query; *cur && ret < 3; ++cur, ++ret)
            if (*cur == '&' || *cur == '|' || *cur == '(' || *cur == ')')
                return ret;
        return ret;
    }

    static TQueryTreeNode* Parse(const char* query, size_t& pos) {
        TQueryTreeNode* node = NULL;

        for (; ; ++pos) {
            size_t control = GetControl(query + pos);
            if (control == 0) {
                if (!query[pos] || query[pos] == ')')
                    break;
                if (query[pos] == '|') {
                    TQueryTreeNode* newNode = Parse(query, ++pos);
                    node = new TQueryOrNode(node, newNode);
                } else if (query[pos] == '(') {
                    TQueryTreeNode* newNode = Parse(query, ++pos);
                    if (!newNode) continue;
                    node = node ? new TQueryAndNode(node, newNode) : newNode;
                }
                // on & just continue
            } else if (control == 3) {
                TTrigram tri = TByte(query[pos]) | (TByte(query[pos + 1]) << 8) | (TByte(query[pos + 2]) << 16);
                TQueryTreeNode* triNode = new TQueryTermNode(tri);
                node = node ? new TQueryAndNode(node, triNode) : triNode;
            } else {
                pos += control - 1;
            }
        }

        return node;
    }
};

} // NCodesearch

