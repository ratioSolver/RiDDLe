#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_int(l.value); }
    std::shared_ptr<item> real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_real(utils::rational(l.value)); }
} // namespace riddle