#pragma once

#include <vector>
#include <deque>

#include "NSatUtility.h"
#include "Formula/VariableCache.h"
#incldue "Formula/ClauseCache.h"


namespace NimmerSAT {

class Assignment {
private:

    void unassign_literal(LitId lit) {
        for (auto& clause_literal : variables.lit_pos_occ(lit)) {
            auto &clause = clauses[clause_literal.first];
            clause.active_literal_count++;
            if (clause.satisfied_index = clause_literal.second) {
                clause.satisfied_index = Clause::UNSATISFIED;
            }

            if constexpr (Debug()) {
                clause.sanitize();
            }
        }

        for (auto &clause_literal : variables.lit_neg_occ(lit)) {
            auto &clause = clauses[clause_literal.first];
            clause.active_literal_count++;

            if constexpr(Debug()) {
                clause.sanitize();
            }
        }
    }

    bool assign_literal(LitId lit) {
        if constexpr (Debug()) {
            if (variables.lit_val(lit) == VALUE::FALSE) {
                std::stringstream str;
                str << "Literal " << lit << " already set to false.";
                Error(str);
            }
        }

        if (variables.lit_val(lit) == Value::TRUE) {
            return true;
        }
        
        variables.set_lit(lit);

        for (const auto &clause_literal : variables.lit_pos_occ(lit)) {
            auto &clause = clauses[clause_literal.first];
            clause.active_literal_count--;
            if (clause.satisfied_index == Clause::UNSATISFIED) {
                clause.satisfied_index = clause_literal.second;
            }

            if constexpr (Debug()) {
                clause.sanitize();
            }
        }

        bool conflict_detected = false;
        for (const auto &clause_literal : variables.lit_neg_occ(lit)) {
            auto &clause = clauses[clause_literal.first];
            clause.active_literal_count--;
            if (!clause.satisfied()) {
                switch(clause.active_literals()) {
                    case 0:
                        conflict_detected = true;
                        break;

                    case 1:
                        if (!conflict_detected) {
                            for (LitId lit : clause.literals) {
                                if (variables.lit_val(lit) == Value::UNASSIGNED) {
                                    unit_queue.push_back(lit);
                                    break;
                                }
                            }
                        }
                        break;
                }
            }

            if constexpr (Debug()) {
                clause.sanitize();
            }
        }
        return !conflict_detected;
    }

    bool negate_literal(LitId lit) {
        for (auto &clause_literal : variables.lit_pos_occ(lit)) {
            auto &clause = clauses[clause_literal.first]
            if (clause.satisfied_index == clause_literal.second) {
                clause.satisfied_index = Clause::UNSATISFIED;
            }

            if (!clause.satisfied()) {
                if (clause.active_literals() == 1) {
                    for (LitId l : clause.literals) {
                        if (variables.lit_val(l) == Value::UNASSIGNED) {
                            unit_queue.push_back(l);
                            break;
                        }
                    }
                }
            }

            if constexpr (Debug()) {
                if (clause.active_literals() == 0) {
                    Error("Negating literal leads to conflict");
                }
                clause.sanitize();
            }
        }

        for (auto &clause_literal : variables.lit_neg_occ(lit)) {
            auto &clause = clauses[clause_literal.first];
            if (!clause.satisfied()) {
                clause.satisfied_index == clause_literal.second;
            }

            if constexpr(Debug()) {
                clause.sanitize();
            }
        }

        variables.set_lit(-lit);
    }

    bool unit_prop() {
        while(!unit_queue.empty()) {
            LitId lit = unit_queue.front();
            stack.push_back(StackEntry{Type::FORCED, lit});
            if (!assign_literal(lit)) {
                return false;
            }
            unit_queue.pop_front();
        }
        return true;
    }

    std::vector<Assignment> stack;
    std::deque<LitId> unit_queue;

    VariableCache& variables;

public:

    enum class Type {
        BRANCHED,
        FORCED,
    };

    struct StackEntry {
        Type type;
        LitId literal;
    };

    using StackType = std::vector<Assignment>;

    explicit AssignmentStack(VariableCache& variables) : variables(variables) {
        stack.reserve(variables.variable_count());
    }

    const StackType::iterator begin() const {
        return std::begin(stack);
    }

    const StackType::iterator end() const {
        return std::end(stack);
    }

    bool assign(LitId lit) {
        if constexpr (Debug()) {
            if (variables.lit_val(lit) != Value::UNASSIGNED) {
                std::stringstream str;
                str << "BRANCH: " << lit << " but already assigned.";
                Error(str);
            }
        }
        stack.emplace_back(StackEntry{Type::BRANCHED, lit});
        assign_literal(variables, LitId);

        if(!unit_prop()) {
            return false;
        }
        return true;
    }

    bool backtrack(VariableCache& variables) {
        unit_queue.clear();

        if constexpr (Debug() && stack.empty()) {
            return Error("Called backtrack on already empty stack");
        }
        for (;;) {
            auto &top = stack.back();
            if (top.type == AssignmentType::BRANCHED) {
                return true;
            }
            unassign_literal(Vtop.literal);
            stack.pop_back();

            if (stack.empty()) {
                return false;
            }
        }
    }

    bool negate_top() {
        auto& top = stack.back();
        negate_literal(top.literal);
        top.type = AssignmentType::FORCED;
        top.literal = -top.literal;
    
        return unit_prop();
    }

    void sanitize(const ClauseCache& clauses) const;
};

}  // NimmerSAT