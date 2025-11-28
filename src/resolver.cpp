#include "resolver.hpp"
#include "flaw.hpp"
#include "core.hpp"
#include <numeric>
#include <algorithm>

namespace riddle
{
    resolver::resolver(flaw &flw, utils::rational &&intrinsic_cost) : flw(flw), intrinsic_cost(intrinsic_cost) {}

    utils::rational resolver::get_estimated_cost() const noexcept
    {
#ifdef H_ADD
        return std::accumulate(preconditions.begin(), preconditions.end(), intrinsic_cost, [](const auto &lhs, const auto &prec)
                               { return lhs + prec->get_estimated_cost(); });
#elif defined(H_MAX)
        return std::max_element(preconditions.begin(), preconditions.end(), [](const auto &lhs, const auto &rhs)
                                { return lhs->get_estimated_cost() < rhs->get_estimated_cost(); })
            ->get()
            ->get_estimated_cost();
#else
        static_assert(false, "No heuristic defined for resolver cost estimation");
#endif
    }

    json::json resolver::to_json() const
    {
        json::json j_resolver{{"flaw", flw.get_id()}, {"intrinsic_cost", riddle::to_json(intrinsic_cost)}};
        if (!preconditions.empty())
        {
            json::json j_preconditions(json::json_type::array);
            for (const auto &p : preconditions)
                j_preconditions.push_back(p->get_id());
            j_resolver["preconditions"] = std::move(j_preconditions);
        }
        return j_resolver;
    }
} // namespace riddle
