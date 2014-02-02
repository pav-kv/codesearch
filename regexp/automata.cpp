#include "automata.h"

#include <deque>
#include <sstream>

namespace NCodesearch {

const char* TFiniteAutomaton::DEFAULT_GRAPH_NAME = "Automaton";

TFiniteAutomaton& TFiniteAutomaton::operator += (const TFiniteAutomaton& rhs) {
    // FIXME: expected exactly one final state
    const size_t mySize = Size(), myFinal = *FinalStates.begin();
    Import(rhs);  // import final states along with transitions
    AddTransition(myFinal, mySize + rhs.StartState);
    SetFinalState(myFinal, false);
    return *this;
}

TFiniteAutomaton& TFiniteAutomaton::operator |= (const TFiniteAutomaton& rhs) {
    // FIXME: expected exactly one final state from both the automata
    const size_t mySize = Size(), myFinal = *FinalStates.begin();
    Import(rhs);
    const size_t newSize = Size();
    Resize(newSize + 2);

    AddTransition(newSize, StartState);
    AddTransition(newSize, mySize + rhs.StartState);
    AddTransition(myFinal, newSize + 1);
    AddTransition(mySize + *rhs.FinalStates.begin(), newSize + 1);

    SetStartState(newSize);
    SetFinalState(myFinal, false);
    SetFinalState(mySize + *rhs.FinalStates.begin(), false);
    SetFinalState(newSize + 1);

    return *this;
}

TFiniteAutomaton& TFiniteAutomaton::Iterate() {
    // FIXME: expected exactly one final state
    const size_t mySize = Size(), myFinal = *FinalStates.begin();
    Resize(mySize + 2);

    AddTransition(mySize, StartState);
    AddTransition(myFinal, mySize + 1);
    AddTransition(myFinal, StartState);
    AddTransition(mySize, mySize + 1);

    SetStartState(mySize);
    SetFinalState(myFinal, false);
    SetFinalState(mySize + 1);

    return *this;
}

TFiniteAutomaton& TFiniteAutomaton::Enumerate() {
    vector<size_t> newIndexes(Size());
    size_t newIndex = 0;

    vector<bool> visited(Size());  // TODO: maybe vector<char> ? Compare performance
    visited[StartState] = true;
    std::deque<size_t> que;
    que.push_front(StartState);
    while (!que.empty()) {
        size_t state = que.front();
        que.pop_front();
        newIndexes[state] = newIndex++;
        const TTransisions& tran = States[state].Transitions;
        TTransisions::const_iterator it = tran.find(EPSILON);
        for (TTransisions::const_iterator it = tran.begin(), end = tran.end(); it != end; ++it)
            for (TStateIdSet::const_iterator stIt = it->second.begin(), stEnd = it->second.end(); stIt != stEnd; ++stIt)
                if (!visited[*stIt]) {
                    visited[*stIt] = true;
                    que.push_back(*stIt);
                }
    }

    // and now enumerate not visited states
    for (size_t i = 0, size = newIndexes.size(); i < size; ++i) {
        if (!visited[i])
            newIndexes[i] == newIndex++;
    }

    vector<TState> newStates(Size());
    for (size_t i = 0, size = Size(); i < size; ++i) {
        TState& newState = newStates[newIndexes[i]];
        newState.Swap(States[i]);
        TTransisions& tran = newState.Transitions;
        for (TTransisions::iterator trIt = tran.begin(), trEnd = tran.end(); trIt != trEnd; ++trIt) {
            // TODO: make incrementing iterator
            TStateIdSet newSet;
            for (TStateIdSet::const_iterator stIt = trIt->second.begin(), stEnd = trIt->second.end(); stIt != stEnd; ++stIt)
                newSet.insert(newIndexes[*stIt]);
            trIt->second.swap(newSet);
        }
    }

    States.swap(newStates);

    StartState = newIndexes[StartState];
    TStateIdSet newFinals;
    for (TStateIdSet::const_iterator it = FinalStates.begin(), end = FinalStates.end(); it != end; ++it)
        newFinals.insert(newIndexes[*it]);
    FinalStates.swap(newFinals);

    return *this;
}

void TFiniteAutomaton::ToGraphviz(std::ostream& output, const char* graphName) const {
    output
        << "digraph " << graphName << " {\n"
        << "  rankdir=LR;\n\n"
        << "  node [shape=circle, style=filled];\n";
    for (size_t i = 0, size = Size(); i < size; ++i) {
        const TState& state = States[i];
        output << "  " << i << " [fillcolor=" << (i == StartState ? "darkolivegreen1" : "gray");
        if (state.IsFinal)
            output << ", shape=doublecircle";
        output << "];\n";
    }
    output << "\n";
    for (size_t i = 0, size = Size(); i < size; ++i) {
        const TTransisions& tran = States[i].Transitions;
        typedef map< size_t, vector<TChar> > TArcLabels;
        TArcLabels labels;
        for (TTransisions::const_iterator trIt = tran.begin(), trEnd = tran.end(); trIt != trEnd; ++trIt)
            for (TStateIdSet::const_iterator stIt = trIt->second.begin(), stEnd = trIt->second.end(); stIt != stEnd; ++stIt)
                labels[*stIt].push_back(trIt->first);
        for (TArcLabels::const_iterator it = labels.begin(), end = labels.end(); it != end; ++it) {
            output << "  " << i << " -> " << it->first << " [label=\"";
            vector<TChar> chars(it->second.begin(), it->second.end());
            for (size_t c = 0, cSize = chars.size(); c < cSize; ++c) {
                if (chars[c] != EPSILON)
                    output << chars[c] << "(" << (char)chars[c] << ")";
                else
                    output << "EPS";
                output << (c + 1 != cSize ? ":" : "");
            }
            output << "\"];\n";
        }
    }
    output << "}\n";
}

std::string TFiniteAutomaton::ToGraphviz(const char* graphName) const {
    std::ostringstream oss;
    ToGraphviz(oss, graphName);
    return oss.str();
}

void TFiniteAutomaton::Import(const TFiniteAutomaton& rhs, bool importFinals) {
    const size_t mySize = Size(), rhsSize = rhs.Size();
    Resize(mySize + rhsSize);
    // NOTE: we don't use vector::end() because self-import is possible
    std::copy(rhs.States.begin(), rhs.States.begin() + rhsSize, States.begin() + mySize);
    for (size_t i = mySize, size = Size(); i < size; ++i) {
        if (!importFinals)
            States[i].IsFinal = false;
        else if (States[i].IsFinal)
            FinalStates.insert(i);

        for (TTransisions::iterator trIt = States[i].Transitions.begin(), trEnd = States[i].Transitions.end(); trIt != trEnd; ++trIt) {
            // TODO: define iterator which returnes incremented values
            TStateIdSet newSet;
            for (TStateIdSet::const_iterator stIt = trIt->second.begin(), stEnd = trIt->second.end(); stIt != stEnd; ++stIt)
                newSet.insert(*stIt + mySize);
            trIt->second.swap(newSet);
        }
    }
}

} // NCodesearch

