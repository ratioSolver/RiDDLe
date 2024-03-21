#include "compilation_unit.hpp"

namespace riddle
{
    void compilation_unit::declare(std::shared_ptr<scope> &scp) const
    {
    }

    void compilation_unit::refine(std::shared_ptr<scope> &scp) const
    {
    }

    void compilation_unit::refine_predicates(std::shared_ptr<scope> &scp) const
    {
    }

    void compilation_unit::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        for (const auto &stmt : body)
            stmt->execute(scp, ctx);
    }
} // namespace riddle