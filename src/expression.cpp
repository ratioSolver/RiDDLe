#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_bool(l.value); }
} // namespace riddle
