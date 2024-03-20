#include "compilation_unit.hpp"

namespace riddle
{
    void compilation_unit::declare(scope &scp) const
    {
    }

    void compilation_unit::refine(scope &scp) const
    {
    }

    void compilation_unit::refine_predicates(scope &scp) const
    {
    }

    void compilation_unit::execute(scope &scp, std::shared_ptr<env> &ctx) const
    {
        for (const auto &stmt : body)
            stmt->execute(scp, ctx);
    }
} // namespace riddle