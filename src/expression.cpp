#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_bool(l->value); }
} // namespace riddle