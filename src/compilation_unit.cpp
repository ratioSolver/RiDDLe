#include "compilation_unit.hpp"
#include <stdexcept>

namespace riddle
{
    void compilation_unit::declare(scope &scp) const { throw std::runtime_error("Not implemented"); }
    void compilation_unit::refine(scope &scp) const { throw std::runtime_error("Not implemented"); }
    void compilation_unit::refine_predicates(scope &scp) const { throw std::runtime_error("Not implemented"); }
    void compilation_unit::execute(const scope &scp, env &ctx) const { throw std::runtime_error("Not implemented"); }
} // namespace riddle