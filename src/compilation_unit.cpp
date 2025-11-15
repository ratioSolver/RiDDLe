#include "compilation_unit.hpp"
#include <stdexcept>

namespace riddle
{
    void compilation_unit::declare(scope &scp) const
    {
        // we declare the types..
        for (const auto &tp : types)
            tp->declare(scp);
        // we declare the predicates..
        for (const auto &p : predicates)
            p->declare(scp);
    }
    void compilation_unit::refine(scope &scp) const
    {
        // we refine the types..
        for (const auto &t : types)
            t->refine(scp);
        // we refine the methods..
        for (const auto &m : methods)
            m->refine(scp);
        // we refine the predicates..
        for (const auto &p : predicates)
            p->refine(scp);
    }
    void compilation_unit::refine_predicates(scope &scp) const
    {
        // we refine the types..
        for (const auto &t : types)
            t->refine_predicates(scp);
    }
    void compilation_unit::execute(const scope &scp, env &ctx) const
    {
        // we execute the statements..
        for (const auto &stmt : body)
            stmt->execute(scp, ctx);
    }
} // namespace riddle