#include "expression.hpp"
#include "core.hpp"
#include "type.hpp"
#include "constructor.hpp"
#include "method.hpp"

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
        auto tp_opt = scp.get_type(instance_type.front().id);
        if (!tp_opt)
            throw std::runtime_error("Cannot find class " + instance_type.front().id);
        auto tp = dynamic_cast<component_type *>(&tp_opt.value().get());
        if (!tp)
            throw std::runtime_error("Class " + instance_type.front().id + " is not a component type");
        for (auto it = instance_type.begin() + 1; it != instance_type.end(); it++)
        {
            tp_opt = tp->get_type(it->id);
            if (!tp_opt)
                throw std::runtime_error("Cannot find class " + it->id);
            tp = dynamic_cast<component_type *>(&tp_opt.value().get());
            if (!tp)
                throw std::runtime_error("Class " + it->id + " is not a component type");
        }

        std::vector<std::reference_wrapper<const type>> arg_types;
        std::vector<std::shared_ptr<item>> arguments;

        for (const auto &arg : args)
        {
            auto xpr = arg->evaluate(scp, ctx);
            arg_types.push_back(xpr->get_type());
            arguments.push_back(xpr);
        }

        return tp->get_constructor(arg_types).invoke(std::move(arguments));
    }

    std::shared_ptr<item> eq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().eq(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }
    std::shared_ptr<item> neq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().negate(scp.get_core().eq(l->evaluate(scp, ctx), r->evaluate(scp, ctx))); }

    std::shared_ptr<item> lt_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().lt(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }
    std::shared_ptr<item> gt_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().gt(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }
    std::shared_ptr<item> leq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().leq(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }
    std::shared_ptr<item> geq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().geq(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }

    std::shared_ptr<item> function_expression::evaluate(scope &scp, env &ctx) const
    {
        auto c_env = &ctx;
        for (const auto &id : object_id)
        {
            auto it = c_env->get(id.id);
            if (!it)
                throw std::runtime_error("Cannot find object " + id.id);
            if (auto cmp = dynamic_cast<component *>(it.get()))
                c_env = cmp;
            else
                throw std::runtime_error("Object " + id.id + " is not a component");
        }

        std::vector<std::reference_wrapper<const type>> arg_types;
        std::vector<std::shared_ptr<item>> arguments;

        for (const auto &arg : args)
        {
            auto xpr = arg->evaluate(scp, ctx);
            arg_types.push_back(xpr->get_type());
            arguments.push_back(xpr);
        }

        auto method_opt = c_env == &scp.get_core() ? scp.get_core().get_method(function_name.id, arg_types) : static_cast<component_type &>(static_cast<component &>(*c_env).get_type()).get_method(function_name.id, arg_types);
        if (!method_opt)
            throw std::runtime_error("Cannot find method " + function_name.id);

        return method_opt.value().get().invoke(*c_env, std::move(arguments));
    }
} // namespace riddle
