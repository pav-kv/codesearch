#include "automata.h"

#include <iostream>

using namespace NCodesearch;

int main() {
    TFiniteAutomaton nine = TFiniteAutomaton::Elementary('9');
    TFiniteAutomaton e = TFiniteAutomaton::Elementary('e');
    TFiniteAutomaton dot = TFiniteAutomaton::Elementary('.');
    TFiniteAutomaton sign = TFiniteAutomaton::Elementary('-');

    TFiniteAutomaton fsm = sign.ZeroOrOne() + +nine + (dot + +nine).ZeroOrOne() + (e + sign.ZeroOrOne() + +nine).ZeroOrOne();
    fsm.Determine();

    fsm.ToGraphviz(std::cout);

    return 0;
}

