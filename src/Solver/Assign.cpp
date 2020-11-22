
#include "Assign.h"
#include "NSatUtility.h"

namespace NimmerSAT {

Solver::Solver(std::uint32_t variable_count, ClauseCache clause_cache) :
    variables(variable_count),
    clauses(std::move(clause_cache))
{
    for (ClauseId clause_id = 0; clause_id < static_cast<ClauseId>(clauses.size()); clause_id++) {
        const auto& current_clause = clauses[clause_id];
        std::uint32_t clause_size = static_cast<std::uint32_t>(current_clause.literals.size());
        
        if (clause_size == 1) {
            unit_queue.push_back(current_clause.literals[0]);
        }

        for (std::uint32_t literal_pos = 0; literal_pos < clause_size; literal_pos++) {
            LitId current_literal = current_clause.literals[literal_pos];
            variables.observe(current_literal, ClauseLiteralIndex{clause_id, literal_pos});
        }
    }
}

bool Solver::assign_literal(LitId lit, AssignmentType type) {
    if constexpr (Debug()) {
        if (type == AssignmentType::BRANCHED && variables.lit_val(lit) != Value::UNASSIGNED) {
            std::stringstream str;
            str << "BRANCH: " << lit << " but already assigned.";
            Error(str);
        }
        if (variables.lit_val(lit) == Value::FALSE) {
            std::stringstream str;
            str << "Literal " << lit << " already set to false.";
            Error(str);
        }
    }

    if (variables.lit_val(lit) == Value::TRUE) {
        return true;
    }

    assignment_stack.push_back(Assignment{type, lit});
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

void Solver::unassign(LitId lit) {

    for (auto &clause_literal : variables.lit_pos_occ(lit)) {
        auto &clause = clauses[clause_literal.first];
        clause.active_literal_count++;
        if (clause.satisfied_index == clause_literal.second) {
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

    variables.unset_lit(lit);
}

void Solver::sanitize() const {

    std::vector<bool> is_already_stacked(variables.variable_count(), false);
    for (const auto &assignemnt : assignment_stack) {
        LitId lit = assignemnt.literal;
        if (is_already_stacked.at(LitToVar(lit) - 1) == true) {
            std::stringstream str;
            str << "Variable " << LitToVar(lit) << " twice in stack.";
            Error(str);
        }
        is_already_stacked[LitToVar(lit) - 1] = true;
        if (variables.lit_val(lit) != Value::TRUE) {
            std::stringstream str;
            str << "Value on stack does not match assigned value" << lit;
            Error(str);
        }
    }

    for (VarId i = 1; i <= variables.variable_count(); i++) {
        if (is_already_stacked[i - 1] == false && variables.var_val(i) != Value::UNASSIGNED) {
            std::stringstream str;
            str << "Variable " << i << " assigned but not on stack";
            Error(str);
        }
    }

    if (assignment_stack.size() > variables.variable_count()) {
        std::stringstream str;
        str << "Assignment stack size: " << assignment_stack.size();
        str << " Variable count: " << variables.variable_count();
        Error(str);
    }

    if (unit_queue.size() > clauses.size()) {
        std::stringstream str;
        str << "Unit queue size: " << unit_queue.size();
        str << " Clause count: " << clauses.size();
        Error(str);
    }

    for (const auto& clause : clauses) {
        std::uint32_t check_active_literal_count = 0;
        for (LitId lit : clause.literals) {
            if (variables.lit_val(lit) == Value::UNASSIGNED) {
                check_active_literal_count++;
            }
        }

        if (check_active_literal_count != clause.active_literals()) {
            std::stringstream str;
            str << "Active literal count does not match: ";
            str << "Expected: " << check_active_literal_count << " Got: " << clause.active_literals();
            Error(str);
        }

        if (clause.satisfied_index != Clause::UNSATISFIED) {
            if (variables.lit_val(clause.literals[clause.satisfied_index]) != Value::TRUE) {
                std::stringstream str;
                str << "Clause satisfied index is not correct";
                Error(str);
            }
        }
    }
}

}  // namespace NimmerSAT
