#pragma once

#include <cstdint>
#include <deque>
#include <vector>
#include <stack>
#include <optional>

#include "Formula/VariableCache.h"
#include "Formula/FormulaCache.h"
#include "NSatUtility.h"

#include <iostream>

namespace NimmerSAT {

class Solver final {
public:

    explicit Solver(std::uint32_t variable_count, ClauseCache clauses);

    Solver(const Solver&) = delete;
    Solver(Solver&&) = default;
    Solver& operator=(const Solver&) = delete;
    Solver& operator=(Solver&&) = default;
    ~Solver() = default;

    bool solve() {
        for(;;) {
            if constexpr(Debug()) {
                sanitize();
            }

            //print_unit();
            if (!unit_prop()) {
                //print_stack();
                backtrack();
                if (assignment_stack.empty()) {
                    return false;
                }
                LitId top_lit = assignment_stack.back().literal;
                unassign(top_lit);
                assignment_stack.pop_back();
                assign_literal(-top_lit, AssignmentType::FORCED);
                //variables.print();
            } else {
                if (!branch()) {
                    return true;
                }
            }
        }
   }

    inline void print() const {
        variables.print();        
    }

private:

    void print_unit() const {
        if (!unit_queue.empty()) {
            std::cout << "UNIT: ";
            for (LitId l : unit_queue) { std::cout << l << ' '; }
            std::cout << std::endl;
        }
    }

    void print_stack() const {
        std::cout << "BACK: ";
        for (const auto &ass : assignment_stack) {
            std::cout << ass.literal 
                    << (ass.type == AssignmentType::FORCED ? "F" : "B") << ' ';
        }
        std::cout << std::endl;
    }

    enum class AssignmentType {
        FORCED,
        BRANCHED
    };

    struct Assignment {
        AssignmentType type;
        LitId literal;
    };

    bool assign_literal(LitId lit, AssignmentType type);

    inline bool unit_prop() {
        while(!unit_queue.empty()) {
            if (LitId lit = unit_queue.front(); 
                !assign_literal(lit, AssignmentType::FORCED)) {
                return false;
            }
            unit_queue.pop_front();
        }
        return true;
    }

    void unassign(LitId lit);

    inline void backtrack() {
        while(!assignment_stack.empty() &&
         assignment_stack.back().type == AssignmentType::FORCED) {
            unassign(assignment_stack.back().literal);
            assignment_stack.pop_back();
        }
        unit_queue.clear();
    }

    inline bool branch() {
        for (VarId i = 1; i <= variables.variable_count(); i++) {
            if (variables.var_val(i) == Value::UNASSIGNED) {
                assign_literal(VarToLit(i, true), AssignmentType::BRANCHED);
                return true;
            }
        }
        return false;
    }

    void sanitize() const;

    std::vector<Assignment> assignment_stack;

    std::deque<LitId> unit_queue;

    VariableCache variables;

    ClauseCache clauses;

};

}  // namespace NimmerSAT
