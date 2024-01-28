#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_int(l.value); }
    std::shared_ptr<item> real_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_real(l.value); }
} // namespace riddle
