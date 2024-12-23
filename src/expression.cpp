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
            return ct->get_constructor(argument_types).invoke(std::move(args));
        else
            throw std::runtime_error("Invalid type reference");
    }
} // namespace riddle