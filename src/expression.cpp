#include "expression.hpp"
#include "core.hpp"

namespace riddle
{
    std::shared_ptr<item> bool_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_bool(l.value); }
    std::shared_ptr<item> int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_int(l.value); }
    std::shared_ptr<item> bounded_int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_int(lb.value, ub.value); }
    std::shared_ptr<item> uncertain_int_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_uncertain_int(lb.value, ub.value); }
    std::shared_ptr<item> real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_real(utils::rational(l.value)); }
    std::shared_ptr<item> bounded_real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_real(utils::rational(lb.value), utils::rational(ub.value)); }
    std::shared_ptr<item> uncertain_real_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_uncertain_real(utils::rational(lb.value), utils::rational(ub.value)); }
    std::shared_ptr<item> string_expression::evaluate(const scope &, env &ctx) const { return ctx.get_core().new_string(std::string(l.value)); }

    std::shared_ptr<item> id_expression::evaluate(const scope &, env &ctx) const
    {
        auto obj = ctx.get(object_id[0].id);
        for (size_t i = 1; i < object_id.size(); ++i)
            if (auto c = dynamic_cast<component *>(obj.get()))
                obj = c->get(object_id[i].id);
            else
                throw std::runtime_error("Invalid object reference");
        return obj;
    }

    std::shared_ptr<item> and_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(std::dynamic_pointer_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_and(std::move(exprs));
    }

    std::shared_ptr<item> or_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(std::dynamic_pointer_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_or(std::move(exprs));
    }

    std::shared_ptr<item> xor_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<std::shared_ptr<bool_item>> exprs;
        for (const auto &expr : xprs)
            exprs.emplace_back(std::dynamic_pointer_cast<bool_item>(expr->evaluate(scp, ctx)));
        return ctx.get_core().new_xor(std::move(exprs));
    }

    std::shared_ptr<item> not_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_not(std::dynamic_pointer_cast<bool_item>(xpr->evaluate(scp, ctx))); }

    std::shared_ptr<item> minus_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_negation(std::dynamic_pointer_cast<arith_item>(xpr->evaluate(scp, ctx))); }

    std::shared_ptr<item> sum_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(std::dynamic_pointer_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_sum(std::move(c_xprs));
    }

    std::shared_ptr<item> product_expression::evaluate(const scope &scp, env &ctx) const
    {
        std::vector<std::shared_ptr<arith_item>> c_xprs;
        for (const auto &xpr : xprs)
            c_xprs.emplace_back(std::dynamic_pointer_cast<arith_item>(xpr->evaluate(scp, ctx)));
        return ctx.get_core().new_product(std::move(c_xprs));
    }

    std::shared_ptr<item> divide_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_divide(std::dynamic_pointer_cast<arith_item>(lhs->evaluate(scp, ctx)), std::dynamic_pointer_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    std::shared_ptr<item> lt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_lt(std::dynamic_pointer_cast<arith_item>(lhs->evaluate(scp, ctx)), std::dynamic_pointer_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    std::shared_ptr<item> le_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_le(std::dynamic_pointer_cast<arith_item>(lhs->evaluate(scp, ctx)), std::dynamic_pointer_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    std::shared_ptr<item> gt_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_gt(std::dynamic_pointer_cast<arith_item>(lhs->evaluate(scp, ctx)), std::dynamic_pointer_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    std::shared_ptr<item> ge_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_ge(std::dynamic_pointer_cast<arith_item>(lhs->evaluate(scp, ctx)), std::dynamic_pointer_cast<arith_item>(rhs->evaluate(scp, ctx))); }

    std::shared_ptr<item> eq_expression::evaluate(const scope &scp, env &ctx) const { return ctx.get_core().new_eq(lhs->evaluate(scp, ctx), rhs->evaluate(scp, ctx)); }

    std::shared_ptr<item> constructor_expression::evaluate(const scope &scp, env &ctx) const
    {
        auto tp = &scp.get_type(type_id[0].id);
        for (size_t i = 1; i < type_id.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(tp))
                tp = &ct->get_type(type_id[i].id);
            else
                throw std::runtime_error("Invalid type reference");

        std::vector<std::shared_ptr<item>> args;
        for (const auto &arg : arguments)
            args.emplace_back(arg->evaluate(scp, ctx));

        std::vector<std::reference_wrapper<const type>> argument_types;
        for (const auto &arg : args)
            argument_types.emplace_back(arg->get_type());

        if (auto ct = dynamic_cast<component_type *>(tp))
        {
            auto instance = ct->new_instance();
            ct->get_constructor(argument_types).invoke(std::dynamic_pointer_cast<component>(instance), std::move(args));
            return instance;
        }
        else
            throw std::runtime_error("Invalid type reference");
    }

    std::shared_ptr<item> call_expression::evaluate(const scope &scp, env &ctx) const
    {
        auto obj = &ctx;
        for (size_t i = 0; i < object_id.size(); ++i)
            obj = static_cast<component *>(obj->get(object_id[i].id).get());

        std::vector<std::shared_ptr<item>> args;
        for (const auto &arg : arguments)
            args.emplace_back(arg->evaluate(scp, ctx));

        std::vector<std::reference_wrapper<const type>> argument_types;
        for (const auto &arg : args)
            argument_types.emplace_back(arg->get_type());

        if (auto c = dynamic_cast<component *>(obj))
            return static_cast<component_type &>(c->get_type()).get_method(function_id.id, argument_types).invoke(*obj, std::move(args));
        else if (auto c = dynamic_cast<core *>(obj))
            return c->get_method(function_id.id, argument_types).invoke(*obj, std::move(args));
        else
            throw std::runtime_error("Invalid object reference");
    }
} // namespace riddle