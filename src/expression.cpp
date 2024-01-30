#include "expression.hpp"
#include "core.hpp"
#include "type.hpp"
#include "constructor.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().new_int(l.val); }
    std::shared_ptr<item> real_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().new_real(l.val); }
    std::shared_ptr<item> string_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().new_string(l.str); }

    std::shared_ptr<item> cast_expression::evaluate(scope &scp, env &ctx) const { return expr->evaluate(scp, ctx); }

    std::shared_ptr<item> plus_expression::evaluate(scope &scp, env &ctx) const { return expr->evaluate(scp, ctx); }
    std::shared_ptr<item> minus_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().minus(expr->evaluate(scp, ctx)); }

    std::shared_ptr<item> not_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().negate(expr->evaluate(scp, ctx)); }

    std::shared_ptr<item> constructor_expression::evaluate(scope &scp, env &ctx) const
    {
        component_type *tp;
        for (auto it = instance_type.begin(); it != instance_type.end(); ++it)
        {
            if (it == instance_type.begin())
                tp = dynamic_cast<component_type *>(&scp.get_type(it->id));
            else
                tp = dynamic_cast<component_type *>(&tp->get_type(it->id));
            if (!tp)
                throw std::runtime_error("Cannot invoke constructor on non-component type " + it->id);
        }

        std::vector<std::reference_wrapper<const type>> arg_types;
        std::vector<std::shared_ptr<item>> arguments;

        for (auto &arg : args)
        {
            auto xpr = arg->evaluate(scp, ctx);
            arg_types.push_back(xpr->get_type());
            arguments.push_back(xpr);
        }

        return tp->get_constructor(arg_types).invoke(std::move(arguments));
    }
} // namespace riddle
