#pragma once

#include "types.h"

#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::vector;

namespace NCodesearch {

// Represents NFA with epsilon transitions
class TFiniteAutomaton {
public:
    typedef set<size_t> TStateIdSet;
    typedef map<TChar, TStateIdSet> TTransitions;

    struct TState {
        // size_t Id;
        bool IsFinal;
        TTransitions Transitions;

        void Swap(TState& other) {
            std::swap(IsFinal, other.IsFinal);
            Transitions.swap(other.Transitions);
        }
    };

    static const char* DEFAULT_GRAPH_NAME;

private:
    vector<TState> States;
    size_t StartState;
    TStateIdSet FinalStates;

public:
    TFiniteAutomaton(size_t size = 1)
        : States(size)
        , StartState(0)
    { /* no-op */ }

    size_t Size() const {
        return States.size();
    }

    size_t Resize(size_t size) {
        size_t oldSize = States.size();
        States.resize(size);
        /*if (size < oldSize)
            for (size_t i = 0; i < size; ++i) {
                TTransitions& tran = States[i].Transitions;
                for (TTransitions::iterator it = tran.begin(); it != tran.end(); ++it)
                    if (tran->second >= size)
            }*/
        return oldSize;
    }

    void AddTransition(size_t from, size_t to, TChar ch = EPSILON) {
        States[from].Transitions[ch].insert(to);
    }

    void SetStartState(size_t id) {
        StartState = id;
    }

    void SetFinalState(size_t id, bool final = true) {
        bool& isFinal = States[id].IsFinal;
        if (isFinal != final) {
            isFinal = final;
            if (final)
                FinalStates.insert(id);
            else
                FinalStates.erase(id);
        }
    }

    bool IsFinalState(size_t id) const {
        return States[id].IsFinal;
    }

    const TStateIdSet& GetFinalStates() const {
        return FinalStates;
    }

    static TFiniteAutomaton Elementary(TChar ch = EPSILON) {
        if (ch == EPSILON) {
            TFiniteAutomaton result(1);
            result.SetFinalState(0);
            return result;
        }
        TFiniteAutomaton result(2);
        result.SetFinalState(1);
        result.AddTransition(0, 1, ch);
        return result;
    }

    TFiniteAutomaton& operator += (const TFiniteAutomaton& rhs); // concatenate
    TFiniteAutomaton& operator |= (const TFiniteAutomaton& rhs); // unite
    TFiniteAutomaton& Iterate(); // Kleene star

    TFiniteAutomaton& Enumerate();

    TFiniteAutomaton operator * () const { TFiniteAutomaton result(*this); return result.Iterate(); }
    TFiniteAutomaton operator + () const {
        // TODO: implement * through + by adding one eps-transition
        //TFiniteAutomaton iter(*this); return iter + *iter;
        TFiniteAutomaton result(*this);
        result.Iterate();
        result.States[result.StartState].Transitions[EPSILON].erase(*result.FinalStates.begin());
        return result;
    }
    TFiniteAutomaton ZeroOrOne() const {
        //return TFiniteAutomaton::Elementary(EPSILON) | *this;
        TFiniteAutomaton result(*this);
        result.AddTransition(result.StartState, *result.FinalStates.begin());
        return result;
    }

    TFiniteAutomaton operator + (const TFiniteAutomaton& rhs) const { TFiniteAutomaton result(*this); return result += rhs; }
    TFiniteAutomaton operator | (const TFiniteAutomaton& rhs) const { TFiniteAutomaton result(*this); return result |= rhs; }

    bool EpsClosure(TStateIdSet& stateSet, vector<bool>& visited) const;  // returns true, iff resulting stateSet contains final states
    TFiniteAutomaton Determined() const;
    TFiniteAutomaton& Determine() { TFiniteAutomaton aut(*this); return *this = aut.Determined(); }

    // TODO: configure colours and shape
    void ToGraphviz(std::ostream& output, const char* graphName = DEFAULT_GRAPH_NAME) const;
    std::string ToGraphviz(const char* graphName = DEFAULT_GRAPH_NAME) const;

private:
    void Import(const TFiniteAutomaton& rhs, bool importFinals = true);
};

} // NCodesearch

