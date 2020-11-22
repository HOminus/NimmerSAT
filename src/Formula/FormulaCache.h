#pragma once

#include <cstdint>
#include <vector>
#include <limits>
#include <cassert>

#include "NSatUtility.h"

namespace NimmerSAT {

struct Clause {

    constexpr static std::uint32_t UNSATISFIED = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t satisfied_index = Clause::UNSATISFIED;
    std::uint32_t active_literal_count;

    std::vector<LitId> literals;

    explicit Clause(std::vector<LitId> literals);

    Clause(const Clause&) = delete;
    Clause(Clause&&) = default;

    Clause& operator=(const Clause&) = delete;
    Clause& operator=(Clause&&) = default;
    ~Clause() = default;

    inline bool satisfied() const noexcept {
        return satisfied_index != UNSATISFIED;
    }

    inline std::uint32_t active_literals() const noexcept {
        return active_literal_count;
    }

    inline void set_satisfied(std::uint32_t index) noexcept(!Debug()) {
        if constexpr (Debug()) {
            if (index >= literals.size()) {
                throw std::exception();
            }
        }
        satisfied_index = index;
    }

    inline void sanitize() const {
        std::uint32_t literal_count = static_cast<std::uint32_t>(this->literals.size());
        if (this->active_literals() > literal_count) {
            std::stringstream str;
            str << "Active literal count greater than literal count";
            str << "Active: " << active_literals() << " Count: " << literal_count;
            Error(str);
        }

        if (this->satisfied_index != Clause::UNSATISFIED &&
        this->satisfied_index > literal_count) {
            Error("Satisfied index invalid");
        }
    }

};

using ClauseCache = std::vector<Clause>;

}  // namespace NimmerSAT
