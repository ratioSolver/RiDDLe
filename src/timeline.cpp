#include "timeline.hpp"
#include "core.hpp"

namespace riddle
{
    std::tuple<std::map<utils::inf_rational, std::unordered_set<atom_expr>>, std::map<utils::inf_rational, std::unordered_set<atom_expr>>, std::set<utils::inf_rational>> timeline::get_pulses(const std::vector<riddle::atom_expr> &atms) const noexcept
    {
        // for each pulse, the atoms starting at that pulse..
        std::map<utils::inf_rational, std::unordered_set<atom_expr>> starting_atoms;
        // for each pulse, the atoms ending at that pulse..
        std::map<utils::inf_rational, std::unordered_set<atom_expr>> ending_atoms;
        // all the pulses of the timeline..
        std::set<utils::inf_rational> pulses;

        for (const auto &atm : atms)
        {
            const auto start = cr.arith_value(atm->get<arith_term>(start_kw));
            const auto end = cr.arith_value(atm->get<arith_term>(end_kw));
            starting_atoms[start].insert(atm);
            ending_atoms[end].insert(atm);
            pulses.insert(start);
            pulses.insert(end);
        }
        pulses.insert(cr.arith_value(cr.env::get<arith_term>(origin_kw)));
        pulses.insert(cr.arith_value(cr.env::get<arith_term>(horizon_kw)));

        return {starting_atoms, ending_atoms, pulses};
    }
} // namespace riddle
