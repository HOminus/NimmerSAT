#include "Dimacs.h"

#include <fstream>
#include <string_view>

#include <iostream>

DimacsResult ReadDimacsFromFile(const char* path) {
    std::ifstream file(path);
    DimacsResult result{0, std::vector<NimmerSAT::Clause>()};
    if (file) {
        while(!file.eof()) {
            if (file.peek() == 'c') {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (file.peek() == 'p') {
                std::uint32_t nclauses;
                file.ignore(6);
                file >> result.variable_count >> nclauses;
                result.clauses.reserve(nclauses);
                file.get();
            } else {
                std::vector<NimmerSAT::LitId> clause;
                int l;
                do {
                    if (int x = file.peek();
                        x == '-' || (x >= '0' && x <= '9')) {
                        file >> l;
                        if (l == 0) {
                            file.get();
                            break;
                        }
                        clause.push_back(l);
                    }
                    file.get();
                } while(!file.eof());
                if (!clause.empty()) {
                    result.clauses.push_back(NimmerSAT::Clause(std::move(clause)));
                }
            }
        }
    }
    return result;
}