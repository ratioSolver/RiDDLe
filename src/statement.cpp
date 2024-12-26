#include "core.hpp"
#include "parser.hpp"
#include "conjunction.hpp"

namespace riddle
{
    void local_field_statement::execute(const scope &scp, env &ctx) const
    { // create local fields in the current environment
        auto tp = &scp.get_type(field_type[0].id);
        for (size_t i = 1; i < field_type.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(tp))
                tp = &ct->get_type(field_type[i].id);
            else
                throw std::runtime_error("Invalid type reference");

        for (auto &[id, expr] : fields)
            if (expr)
            { // initialize with an expression
                auto val = expr->evaluate(scp, ctx);
                if (tp->is_assignable_from(val->get_type()))
                    ctx.items.emplace(id.id, val);
                else
                    throw std::runtime_error("Invalid assignment");
            }
            else if (tp->is_primitive()) // initialize with a default value
                ctx.items.emplace(id.id, tp->new_instance());
            else if (auto ct = dynamic_cast<component_type *>(tp))
                switch (ct->get_instances().size())
                {
                case 0: // no instances
                    throw inconsistency_exception();
                case 1: // only one instance
                    ctx.items.emplace(id.id, *ct->get_instances().begin());
                    break;
                default:
                { // multiple instances
                    std::vector<std::reference_wrapper<utils::enum_val>> values;
                    for (auto &inst : ct->get_instances())
                        values.emplace_back(*inst);
                    ctx.items.emplace(id.id, ctx.get_core().new_enum(*ct, std::move(values)));
                }
                }
            else if (auto et = dynamic_cast<enum_type *>(tp))
                switch (et->get_domain().size())
                {
                case 0: // no values
                    throw inconsistency_exception();
                case 1: // only one value
                    ctx.items.emplace(id.id, *et->get_domain().begin());
                    break;
                default:
                { // multiple values
                    std::vector<std::reference_wrapper<utils::enum_val>> values;
                    for (auto &val : et->get_domain())
                        values.emplace_back(*val);
                    ctx.items.emplace(id.id, ctx.get_core().new_enum(*et, std::move(values)));
                }
                }
            else
                throw std::runtime_error("Invalid type reference");
    }

    void assignment_statement::execute(const scope &scp, env &ctx) const
    { // assign a value to a field of an object
        auto obj = ctx.items.find(object_id[0].id);
        if (obj == ctx.items.end())
            throw std::runtime_error("Object not found");

        auto tp = &obj->second->get_type();
        for (size_t i = 1; i < object_id.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(tp))
                tp = &ct->get_type(object_id[i].id);
            else
                throw std::runtime_error("Invalid type reference");

        if (auto ct = dynamic_cast<component_type *>(tp))
        {
            auto field = ct->get_field(field_id.id);
            if (field.get_type().is_assignable_from(value->evaluate(scp, ctx)->get_type()))
                static_cast<component &>(*obj->second).items.emplace(field_id.id, value->evaluate(scp, ctx));
            else
                throw std::runtime_error("Invalid assignment");
        }
        else
            throw std::runtime_error("Invalid type reference");
    }

    void expression_statement::execute(const scope &scp, env &ctx) const
    { // evaluate an expression and assert it as a fact
        scp.get_core().assert_fact(std::dynamic_pointer_cast<bool_item>(xpr->evaluate(scp, ctx)));
    }

    void conjunction_statement::execute(const scope &scp, env &ctx) const
    { // execute a conjunction of statements
        for (auto &stmt : stmts)
            stmt->execute(scp, ctx);
    }

    void disjunction_statement::execute(const scope &scp, env &ctx) const
    { // execute a disjunction of conjunctions
        std::vector<std::unique_ptr<conjunction>> conjs;
        for (auto &conj : blocks)
        {
            env cctx(ctx);
            auto cst = conj->cst ? scp.get_core().arith_value(static_cast<arith_item &>(*conj->cst->evaluate(scp, ctx))).get_rational() : utils::rational::one;
            conjs.emplace_back(std::make_unique<conjunction>(scp, std::move(cctx), cst, conj->stmts));
        }
        scp.get_core().new_disjunction(std::move(conjs));
    }

    void for_all_statement::execute(const scope &scp, env &ctx) const
    { // execute a for-all statement
        auto tp = &scp.get_type(enum_type[0].id);
        for (size_t i = 1; i < enum_type.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(tp))
                tp = &ct->get_type(enum_type[i].id);
            else
                throw std::runtime_error("Invalid type reference");
        if (auto ct = dynamic_cast<component_type *>(tp))
            for (auto &inst : ct->get_instances())
            {
                env cctx(ctx);
                cctx.items.emplace(enum_id.id, *inst);
                for (auto &stmt : stmts)
                    stmt->execute(scp, cctx);
            }
        else
            throw std::runtime_error("Invalid type reference");
    }
} // namespace riddle
