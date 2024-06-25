#include "expression.hpp"
#include "core.hpp"
#include "type.hpp"
#include "constructor.hpp"
#include "method.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(const scope &scp, std::shared_ptr<env>) const { return scp.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(const scope &scp, std::shared_ptr<env>) const { return scp.get_core().new_int(l.val); }
    std::shared_ptr<item> real_expression::evaluate(const scope &scp, std::shared_ptr<env>) const { return scp.get_core().new_real(l.val); }
    std::shared_ptr<item> string_expression::evaluate(const scope &scp, std::shared_ptr<env>) const { return scp.get_core().new_string(l.str); }

    std::shared_ptr<item> cast_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return expr->evaluate(scp, ctx); }

    std::shared_ptr<item> plus_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return expr->evaluate(scp, ctx); }
    std::shared_ptr<item> minus_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().minus(std::static_pointer_cast<arith_item>(expr->evaluate(scp, ctx))); }

    std::shared_ptr<item> not_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().negate(std::static_pointer_cast<bool_item>(expr->evaluate(scp, ctx))); }

    std::shared_ptr<item> constructor_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        auto tp_opt = scp.get_type(instance_type.front().id);
        if (!tp_opt)
            throw std::runtime_error("Cannot find class " + instance_type.front().id);
        auto tp = dynamic_cast<component_type *>(&tp_opt->get());
        if (!tp)
            throw std::runtime_error("Class " + instance_type.front().id + " is not a component type");
        for (auto it = instance_type.begin() + 1; it != instance_type.end(); it++)
        {
            tp_opt = tp->get_type(it->id);
            if (!tp_opt)
                throw std::runtime_error("Cannot find class " + it->id);
            tp = dynamic_cast<component_type *>(&tp_opt->get());
            if (!tp)
                throw std::runtime_error("Class " + it->id + " is not a component type");
        }

        std::vector<std::shared_ptr<item>> arguments;
        for (const auto &arg : args)
            arguments.push_back(arg->evaluate(scp, ctx));

        return scp.get_core().new_item(*tp, std::move(arguments));
    }

    std::shared_ptr<item> eq_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().eq(l->evaluate(scp, ctx), r->evaluate(scp, ctx)); }
    std::shared_ptr<item> neq_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().negate(scp.get_core().eq(l->evaluate(scp, ctx), r->evaluate(scp, ctx))); }

    std::shared_ptr<item> lt_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().lt(std::static_pointer_cast<arith_item>(l->evaluate(scp, ctx)), std::static_pointer_cast<arith_item>(r->evaluate(scp, ctx))); }
    std::shared_ptr<item> gt_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().gt(std::static_pointer_cast<arith_item>(l->evaluate(scp, ctx)), std::static_pointer_cast<arith_item>(r->evaluate(scp, ctx))); }
    std::shared_ptr<item> leq_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().leq(std::static_pointer_cast<arith_item>(l->evaluate(scp, ctx)), std::static_pointer_cast<arith_item>(r->evaluate(scp, ctx))); }
    std::shared_ptr<item> geq_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const { return scp.get_core().geq(std::static_pointer_cast<arith_item>(l->evaluate(scp, ctx)), std::static_pointer_cast<arith_item>(r->evaluate(scp, ctx))); }

    std::shared_ptr<item> function_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        auto c_env = ctx;
        for (const auto &id : object_id)
        {
            auto itm = c_env->get(id.id);
            if (!itm)
                throw std::runtime_error("Cannot find object " + id.id);
            if (auto cmp = std::dynamic_pointer_cast<component>(itm))
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

        auto method_opt = c_env.get() == &scp.get_core() ? scp.get_core().get_method(function_name.id, arg_types) : static_cast<component_type &>(static_cast<component &>(*c_env).get_type()).get_method(function_name.id, arg_types);
        if (!method_opt)
            throw std::runtime_error("Cannot find method " + function_name.id);

        return method_opt->get().invoke(c_env, std::move(arguments));
    }

    std::shared_ptr<item> id_expression::evaluate(const scope &, std::shared_ptr<env> ctx) const
    {
        auto c_env = ctx->get(object_id.front().id);
        if (!c_env)
            throw std::runtime_error("Cannot find object " + object_id.front().id);
        for (auto it = object_id.begin() + 1; it != object_id.end(); it++)
            if (auto e = dynamic_cast<env *>(c_env.get()))
            {
                c_env = e->get(it->id);
                if (!c_env)
                    throw std::runtime_error("Cannot find object " + it->id);
            }
            else
                throw std::runtime_error("Object " + it->id + " is not an environment");

        return c_env;
    }

    std::shared_ptr<item> implication_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        exprs.push_back(scp.get_core().negate(std::static_pointer_cast<bool_item>(l->evaluate(scp, ctx))));
        exprs.push_back(std::static_pointer_cast<bool_item>(r->evaluate(scp, ctx)));
        return scp.get_core().disj(exprs);
    }

    std::shared_ptr<item> disjunction_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<bool_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().disj(exprs);
    }

    std::shared_ptr<item> conjunction_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<bool_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().conj(exprs);
    }

    std::shared_ptr<item> xor_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<bool_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().exct_one(exprs);
    }

    std::shared_ptr<item> addition_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<arith_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().add(exprs);
    }

    std::shared_ptr<item> subtraction_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<arith_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().sub(exprs);
    }

    std::shared_ptr<item> multiplication_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<arith_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().mul(exprs);
    }

    std::shared_ptr<item> division_expression::evaluate(const scope &scp, std::shared_ptr<env> ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> exprs;
        for (const auto &arg : args)
            exprs.push_back(std::static_pointer_cast<arith_item>(arg->evaluate(scp, ctx)));
        return scp.get_core().div(exprs);
    }
} // namespace riddle
