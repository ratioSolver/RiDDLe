#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_int(l.val); }
    std::shared_ptr<item> real_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_real(l.val); }
    std::shared_ptr<item> string_expression::evaluate(scope &scp, env &ctx) { return scp.get_core().new_string(l.str); }
} // namespace riddle
