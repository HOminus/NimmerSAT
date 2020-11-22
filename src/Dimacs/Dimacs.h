#pragma once

#include <string>
#include "Formula/FormulaCache.h"

struct DimacsResult {
    std::uint32_t variable_count;
    std::vector<NimmerSAT::Clause> clauses;
};

DimacsResult ReadDimacsFromFile(const char* path);
