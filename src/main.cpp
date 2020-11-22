#include <iostream>

#include "Dimacs/Dimacs.h"
#include "Formula/FormulaCache.h"
#include "Formula/VariableCache.h"
#include "Solver/Assign.h"

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        std::cout << "Please specify an argument." << std::endl;
        return -1;
    }

    auto dimacs = ReadDimacsFromFile(argv[1]);

    /*
    for(auto& clause : dimacs.clauses) {
        for (auto literal : clause.literals) {
            std::cout << literal << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
    */

    if (dimacs.variable_count == 0 || dimacs.clauses.size() == 0) {
        std::cout << "Invalid path. Formula is empty." << std::endl;
        return 1;
    }

    NimmerSAT::Solver solver(dimacs.variable_count, std::move(dimacs.clauses));
    if (solver.solve()){
        std::cout << "S" << std::endl;
        solver.print();
    } else {
        std::cout << "U" << std::endl;
    }
}
