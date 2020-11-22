
#include "FormulaCache.h"
#include "VariableCache.h"


namespace NimmerSAT {

    VariableCache::VariableCache(std::uint32_t size) : variables(size) {
    }

    Clause::Clause(std::vector<LitId> literals) : literals(std::move(literals)) {
        active_literal_count = this->literals.size();
    }

}



