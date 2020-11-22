#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>
#include <cmath>
#include <memory>

#include "NSatUtility.h"

#include <iostream>


namespace NimmerSAT {

class VariableCache final {
public:

    explicit VariableCache(std::uint32_t size);

    VariableCache(const VariableCache&) = delete;
    VariableCache(VariableCache&&) = default;

    VariableCache& operator=(const VariableCache&) = delete;
    VariableCache& operator=(VariableCache&&) = default;

    ~VariableCache() = default;

    inline Value lit_val(LitId index) const {
        if (index < 0) {
            return Neg(variables[-index - 1].val);
        }
        return variables[index - 1].val;
    }

    inline Value var_val(VarId index) const {
        return variables[index - 1].val;
    }

    inline const std::vector<ClauseLiteralIndex>& lit_pos_occ(LitId index) const {
        if (index < 0) {
            return variables[-index - 1].neg_occ;
        }
        return variables[index - 1].pos_occ;
    }

    inline std::vector<ClauseLiteralIndex>& lit_pos_occ(LitId index) {
        if (index < 0) {
            return variables[-index - 1].neg_occ;
        }
        return variables[index - 1].pos_occ;
    }

    inline const std::vector<ClauseLiteralIndex>& lit_neg_occ(LitId index) const {
        if (index < 0) {
            return variables[-index - 1].pos_occ;
        }
        return variables[index - 1].neg_occ;
    }

    inline std::vector<ClauseLiteralIndex>& lit_neg_occ(LitId index) {
        if (index < 0) {
            return variables[-index - 1].pos_occ;
        }
        return variables[index - 1].neg_occ;
    }

    inline void observe(LitId literal_index, ClauseLiteralIndex index) {
        lit_pos_occ(literal_index).push_back(index);
    }

    inline const std::vector<ClauseLiteralIndex>& var_pos_occ(VarId index) const {
        return variables[index - 1].pos_occ;
    }

    inline std::vector<ClauseLiteralIndex>& var_pos_occ(VarId index) {
        return variables[index - 1].pos_occ;
    }

    inline const std::vector<ClauseLiteralIndex>& var_neg_occ(VarId index) const {
        return variables[index - 1].neg_occ;
    }

    inline std::vector<ClauseLiteralIndex>& var_neg_occ(VarId index) {
        return variables[index - 1].neg_occ;
    }

    inline void set_var(VarId var, Value val) {
        variables[var - 1].val = val;
    }

    inline void set_lit(LitId lit) {
        if (lit < 0) {
            variables[-lit - 1].val = Value::FALSE;
        } else {
            variables[lit - 1].val = Value::TRUE;
        }
    }

    inline void unset_lit(LitId lit) {
        set_var(LitToVar(lit), Value::UNASSIGNED);
    }

    inline std::uint32_t variable_count() const {
        return variables.size();
    }

    void print() const {
        std::cout << "VARS: ";
        for (std::uint32_t i = 1; i <= variable_count(); i++) {
            switch (var_val(i)) {
                case Value::TRUE:
                    std::cout << ' ' << i << ' ';
                    break;

                case Value::FALSE:
                    std::cout << -static_cast<std::int32_t>(i) << ' ';
                    break;

                case Value::UNASSIGNED:
                    std::cout << 'x' << i << ' ';
                    break;
            }
        }
        std::cout << std::endl;
    }

private:

    struct Variable {
        Value val = Value::UNASSIGNED;

        std::vector<ClauseLiteralIndex> pos_occ;
        std::vector<ClauseLiteralIndex> neg_occ;
    };

    std::vector<Variable> variables;

};

}  // namespace NimmerSAT

