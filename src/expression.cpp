#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    expr bool_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_bool(l.value); }
    expr int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_int(l.value); }
    expr bounded_int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_int(lb.value, ub.value); }
    expr uncertain_int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_uncertain_int(lb.value, ub.value); }
    expr real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_real(utils::rational(l.value)); }
    expr bounded_real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_real(utils::rational(lb.value), utils::rational(ub.value)); }
    expr uncertain_real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_uncertain_real(utils::rational(lb.value), utils::rational(ub.value)); }
    expr string_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_string(std::string(l.value)); }

    expr id_expression::evaluate(const scope &, env &ctx) const
    {
        auto obj = ctx.get(object_id[0].id);
        for (size_t i = 1; i < object_id.size(); ++i)
            if (auto c = dynamic_cast<component *>(obj.get()))
                obj = c->get(object_id[i].id);
            else if (auto c = dynamic_cast<atom_term *>(obj.get()))
                obj = c->get(object_id[i].id);
            else if (auto c = dynamic_cast<enum_term *>(obj.get()))
                obj = c->get(object_id[i].id);
            else
                throw std::runtime_error("Invalid object reference");
        return obj;
    }

    expr and_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_term>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_and(std::move(exprs));
    }

    expr or_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_term>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_or(std::move(exprs));
    }

    expr xor_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_term>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_xor(std::move(exprs));
    }

    expr not_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_not(utils::s_ptr_cast<bool_term>(xpr->evaluate(scp, ctx))); }

    expr minus_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_negation(utils::s_ptr_cast<arith_term>(xpr->evaluate(scp, ctx))); }

    expr sum_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_term>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_sum(std::move(c_xprs));
    }

    expr subtraction_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_term>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_subtraction(std::move(c_xprs));
    }

    expr product_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_term>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_product(std::move(c_xprs));
    }

    expr division_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_term>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_division(std::move(c_xprs));
    }

    expr lt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_lt(utils::s_ptr_cast<arith_term>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_term>(rhs->evaluate(scp, ctx))); }

    expr le_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_le(utils::s_ptr_cast<arith_term>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_term>(rhs->evaluate(scp, ctx))); }

    expr gt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_gt(utils::s_ptr_cast<arith_term>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_term>(rhs->evaluate(scp, ctx))); }

    expr ge_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_ge(utils::s_ptr_cast<arith_term>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_term>(rhs->evaluate(scp, ctx))); }

    expr eq_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_eq(lhs->evaluate(scp, ctx), rhs->evaluate(scp, ctx)); }

    expr constructor_expression::evaluate(const scope &scp, env &ctx) const
    {
        auto tp = &scp.get_type(type_id[0].id);
        for (size_t i = 1; i < type_id.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(tp))
                tp = &ct->get_type(type_id[i].id);
            else
                throw std::runtime_error("Invalid type reference");

        std::vector<expr> args;
        for (const auto &arg : arguments)
            args.emplace_back(arg->evaluate(scp, ctx));

        std::vector<utils::ref_wrapper<const type>> argument_types;
        for (const auto &arg : args)
            argument_types.emplace_back(arg->get_type());

        if (auto ct = dynamic_cast<component_type *>(tp))
        {
            auto instance = ct->new_instance();
            ct->get_constructor(argument_types).invoke(utils::s_ptr_cast<component>(instance), std::move(args));
            return instance;
        }
        else
            throw std::runtime_error("Invalid type reference");
    }

    expr call_expression::evaluate(const scope &scp, env &ctx) const
    {
        expr obj;
        if (!object_id.empty())
        {
            obj = ctx.get(object_id[0].id);
            for (size_t i = 1; i < object_id.size(); ++i)
                if (auto c = dynamic_cast<component *>(obj.get()))
                    obj = c->get(object_id[i].id);
                else if (auto c = dynamic_cast<atom_term *>(obj.get()))
                    obj = c->get(object_id[i].id);
                else if (auto c = dynamic_cast<enum_term *>(obj.get()))
                    obj = c->get(object_id[i].id);
                else
                    throw std::runtime_error("Invalid object reference");
        }

        std::vector<expr> args;
        for (const auto &arg : arguments)
            args.emplace_back(arg->evaluate(scp, ctx));

        std::vector<utils::ref_wrapper<const type>> argument_types;
        for (const auto &arg : args)
            argument_types.emplace_back(arg->get_type());

        if (auto c = dynamic_cast<component *>(&*obj))
            return static_cast<component_type &>(c->get_type()).get_method(function_id.id, argument_types).invoke(obj, std::move(args));
        else if (auto c = dynamic_cast<core *>(&*obj))
            return c->get_method(function_id.id, argument_types).invoke(obj, std::move(args));
        else
            throw std::runtime_error("Invalid object reference");
    }
} // namespace riddle