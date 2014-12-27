#include "automata.h"

#include <iostream>
#include <string>

using namespace NCodesearch;

int main(int argc, char** argv) {
    bool determ = false;
    bool minimize = false;
    if (argc > 1) {
        std::string arg(argv[1]);
        for (size_t i = 0; i < arg.size(); ++i) {
            if (arg[i] == 'm')
                minimize = true;
            else if (arg[i] == 'd')
                determ = true;
        }
    }
    TFiniteAutomaton nums = TFiniteAutomaton::Elementary('0') | TFiniteAutomaton::Elementary('9');
    TFiniteAutomaton exp = TFiniteAutomaton::Elementary('e') | TFiniteAutomaton::Elementary('E');
    TFiniteAutomaton dot = TFiniteAutomaton::Elementary('.');
    TFiniteAutomaton signs = TFiniteAutomaton::Elementary('-') | TFiniteAutomaton::Elementary('+');

    /*TFiniteAutomaton fsm = signs.ZeroOrOne()
        + ((+nums + (dot + *nums).ZeroOrOne()) | (*nums + dot + +nums))
        + (exp + signs.ZeroOrOne() + +nums).ZeroOrOne();*/
    TFiniteAutomaton fsm = signs.ZeroOrOne()
        + +nums + (dot + *nums).ZeroOrOne()
        + (exp + signs.ZeroOrOne() + +nums).ZeroOrOne();
    if (determ)
        fsm.Determine();
    if (minimize)
        fsm.Minimize();

    std::cerr << "Empty? " << fsm.IsEmpty() << '\n';
    TFiniteAutomaton empty(2);
    empty.AddTransition(0, 1);
    std::cerr << "Empty? " << empty.IsEmpty() << '\n';

    fsm.Enumerate();
    fsm.ToGraphviz(std::cout);

    return 0;
}

