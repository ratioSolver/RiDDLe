#include "resolver.hpp"
#include "flaw.hpp"
#include "core.hpp"

namespace riddle
{
    resolver::resolver(flaw &flw, utils::rational &&intrinsic_cost) : flw(flw), intrinsic_cost(intrinsic_cost) {}

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
