#pragma once

#include <cstdint>
#include <utility>
#include <cmath>

#include <iostream>
#include <sstream>

namespace NimmerSAT {

    using VarId = std::uint32_t;
    using LitId = std::int32_t;
    using ClauseId = std::uint32_t;
    using ClauseLiteralIndex = std::pair<ClauseId, std::uint32_t>;

    inline VarId LitToVar(LitId lit_id) {
        return std::abs(lit_id);
    }

    inline LitId VarToLit(VarId var_id, bool pos) {
        LitId lit = static_cast<LitId>(var_id);
        if (!pos) {
            lit = -lit;
        }
        return lit;
    }

    inline LitId Neg(LitId lit_id) {
        return -lit_id;
    }

    enum class Value{
        UNASSIGNED,
        TRUE,
        FALSE
    };

    inline Value Neg(Value val) {
        switch(val) {
            case Value::UNASSIGNED:
                return Value::UNASSIGNED;

            case Value::TRUE:
                return Value::FALSE;

            case Value::FALSE:
                return Value::TRUE;
        }
    }

    inline void Error(std::string str = {}) {
        if (!str.empty()) {
            std::cout << str << std::endl;
        }
        std::abort();
    }

    inline void Error(const std::stringstream& str) {
        auto string = str.str();
        if (!string.empty()) {
            std::cout << string << std::endl;
        }
        std::abort();
    }

    constexpr bool Debug() {
        #ifdef NIMMERSAT_DEBUG
            return true;
        #else
            return false;
        #endif
    }

}  // namespace NimmerSAT
