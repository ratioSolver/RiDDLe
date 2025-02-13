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
            else if (auto c = dynamic_cast<atom *>(obj.get()))
                obj = c->get(object_id[i].id);
            else
                throw std::runtime_error("Invalid object reference");
        return obj;
    }

    expr and_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_and(std::move(exprs));
    }

    expr or_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_or(std::move(exprs));
    }

    expr xor_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<bool_expr> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(utils::s_ptr_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_xor(std::move(exprs));
    }

    expr not_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_not(utils::s_ptr_cast<bool_item>(xpr->evaluate(scp, ctx))); }

    expr minus_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_negation(utils::s_ptr_cast<arith_item>(xpr->evaluate(scp, ctx))); }

    expr sum_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_sum(std::move(c_xprs));
    }

    expr subtraction_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_subtraction(std::move(c_xprs));
    }

    expr product_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_product(std::move(c_xprs));
    }

    expr division_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<arith_expr> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(utils::s_ptr_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_division(std::move(c_xprs));
    }

    expr lt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_lt(utils::s_ptr_cast<arith_item>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    expr le_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_le(utils::s_ptr_cast<arith_item>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    expr gt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_gt(utils::s_ptr_cast<arith_item>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    expr ge_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_ge(utils::s_ptr_cast<arith_item>(lhs->evaluate(scp, ctx)), utils::s_ptr_cast<arith_item>(rhs->evaluate(scp, ctx))); }

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
        auto obj = &ctx;
        for (size_t i = 0; i < object_id.size(); ++i)
            obj = static_cast<component *>(obj->get(object_id[i].id).get());

        std::vector<expr> args;
        for (const auto &arg : arguments)
            args.emplace_back(arg->evaluate(scp, ctx));

        std::vector<utils::ref_wrapper<const type>> argument_types;
        for (const auto &arg : args)
            argument_types.emplace_back(arg->get_type());

        if (auto c = dynamic_cast<component *>(obj))
            return static_cast<component_type &>(c->get_type()).get_method(function_id.id, argument_types).invoke(*obj, std::move(args));
        else if (auto c = dynamic_cast<core *>(obj))
            return c->get_method(function_id.id, argument_types).invoke(*obj, std::move(args));
        else
            throw std::runtime_error("Invalid object reference");
    }

    utils::s_ptr<expression> push_negations(utils::s_ptr<expression> expr) noexcept
    {
        if (auto not_xpr = utils::s_ptr_cast<not_expression>(expr))
        {
            if (auto not_xpr_xpr = utils::s_ptr_cast<not_expression>(not_xpr->xpr))
                return push_negations(not_xpr_xpr->xpr);
            else if (auto and_xpr = utils::s_ptr_cast<and_expression>(not_xpr->xpr))
            {
                std::vector<utils::s_ptr<expression>> args;
                for (const auto &arg : and_xpr->xprs)
                    args.push_back(push_negations(utils::make_s_ptr<not_expression>(arg)));
                return utils::make_s_ptr<or_expression>(std::move(args));
            }
            else if (auto or_xpr = utils::s_ptr_cast<or_expression>(not_xpr->xpr))
            {
                std::vector<utils::s_ptr<expression>> args;
                for (const auto &arg : or_xpr->xprs)
                    args.push_back(push_negations(utils::make_s_ptr<not_expression>(arg)));
                return utils::make_s_ptr<and_expression>(std::move(args));
            }
            else
                return not_xpr;
        }
        else if (auto and_xpr = utils::s_ptr_cast<and_expression>(expr))
        {
            std::vector<utils::s_ptr<expression>> args;
            for (const auto &arg : and_xpr->xprs)
                args.push_back(push_negations(arg));
            return utils::make_s_ptr<and_expression>(std::move(args));
        }
        else if (auto or_xpr = utils::s_ptr_cast<or_expression>(expr))
        {
            std::vector<utils::s_ptr<expression>> args;
            for (const auto &arg : or_xpr->xprs)
                args.push_back(push_negations(arg));
            return utils::make_s_ptr<or_expression>(std::move(args));
        }
        else
            return expr;
    }

    utils::s_ptr<expression> distribute(utils::s_ptr<expression> expr) noexcept
    {
        if (auto or_xpr = utils::s_ptr_cast<or_expression>(expr))
        {
            std::vector<utils::s_ptr<expression>> args;
            for (const auto &arg : or_xpr->xprs)
                args.push_back(distribute(arg));
            std::vector<utils::s_ptr<expression>> new_args;
            for (const auto &arg : args)
                if (auto and_xpr = utils::s_ptr_cast<and_expression>(arg))
                    for (const auto &a : and_xpr->xprs)
                        new_args.push_back(a);
                else
                    new_args.push_back(arg);
            return utils::make_s_ptr<or_expression>(std::move(new_args));
        }
        else if (auto and_xpr = utils::s_ptr_cast<and_expression>(expr))
        {
            std::vector<utils::s_ptr<expression>> args;
            for (const auto &arg : and_xpr->xprs)
                args.push_back(distribute(arg));
            return utils::make_s_ptr<and_expression>(std::move(args));
        }
        else
            return expr;
    }
} // namespace riddle