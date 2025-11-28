#pragma once

#include "json.hpp"
#include "term.hpp"
#include "inf_rational.hpp"
#include <map>
#include <unordered_set>
#include <set>

namespace riddle
{
  class core;

  class timeline
  {
  public:
    timeline(core &cr) noexcept : cr(cr) {}
    virtual ~timeline() = default;

    [[nodiscard]] virtual json::json extract() const = 0;

  protected:
    /**
     * @brief Computes the pulse points for the given set of atom expressions.
     *
     * This function analyzes the provided vector of atom expressions and determines
     * the pulse points associated with them. It returns a tuple containing:
     * - A map from inf_rational values to unordered sets of atom expressions representing the start pulses.
     * - A map from inf_rational values to unordered sets of atom expressions representing the end pulses.
     * - A set of inf_rational values representing all unique pulse points.
     *
     * @param atms The vector of atom expressions to analyze.
     * @return A tuple containing:
     *   - std::map<utils::inf_rational, std::unordered_set<atom_expr>>: Start pulses.
     *   - std::map<utils::inf_rational, std::unordered_set<atom_expr>>: End pulses.
     *   - std::set<utils::inf_rational>: All unique pulse points.
     */
    [[nodiscard]] std::tuple<std::map<utils::inf_rational, std::unordered_set<atom_expr>>, std::map<utils::inf_rational, std::unordered_set<atom_expr>>, std::set<utils::inf_rational>> get_pulses(const std::vector<riddle::atom_expr> &atms) const noexcept;

  private:
    core &cr;
  };
} // namespace riddle
