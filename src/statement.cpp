#include "core.hpp"
#include "parser.hpp"
#include "conjunction.hpp"
#include "exceptions.hpp"
#include <queue>
#include <cassert>

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
        {
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
            else if (auto et = dynamic_cast<enum_type *>(tp))
                ctx.items.emplace(id.id, et->new_instance());
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
            else
                throw std::runtime_error("Invalid type reference");

            if (auto cr = dynamic_cast<core *>(&ctx)) // we have a global field..
                cr->add_field(std::make_unique<field>(*tp, std::string(id.id), nullptr));
        }
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
    {
        auto val = to_cnf(std::dynamic_pointer_cast<bool_term>(xpr->evaluate(scp, ctx))); // convert the expression to conjunctive normal form..
        if (auto and_val = std::dynamic_pointer_cast<and_term>(val))
            for (auto &arg : and_val->args)
            { // we assert each clause
                if (auto or_val = std::dynamic_pointer_cast<or_term>(arg))
                {
                    std::vector<bool_expr> args;
                    for (auto &val : or_val->args)
                        args.emplace_back(std::dynamic_pointer_cast<bool_term>(val));
                    scp.get_core().new_clause(std::move(args));
                }
            }
        else if (auto or_val = std::dynamic_pointer_cast<or_term>(val))
        { // we assert the single clause
            std::vector<bool_expr> args;
            for (auto &val : or_val->args)
                args.emplace_back(std::dynamic_pointer_cast<bool_term>(val));
            scp.get_core().new_clause(std::move(args));
        }
        else
            scp.get_core().new_clause({val});
    }

    void conjunction_statement::execute(const scope &scp, env &ctx) const
    { // execute a conjunction of statements
        for (auto &stmt : stmts)
            stmt->execute(scp, ctx);
    }

    void disjunction_statement::execute(const scope &scp, env &ctx) const
    { // execute a disjunction of conjunctions
        std::vector<std::unique_ptr<conjunction>> conjs;
        std::map<std::string, expr, std::less<>> items;
        env *tmp_ctx = &ctx; // find the nearest core, component or atom
        while (!(dynamic_cast<core *>(tmp_ctx) || dynamic_cast<component *>(tmp_ctx) || dynamic_cast<enum_term *>(tmp_ctx) || dynamic_cast<atom_term *>(tmp_ctx)))
        {
            items.insert(tmp_ctx->items.begin(), tmp_ctx->items.end());
            tmp_ctx = &tmp_ctx->get_parent();
        }
        for (auto &conj : blocks)
        {
            env cctx(scp.get_core(), *tmp_ctx);            // we create a new context
            cctx.items.insert(items.begin(), items.end()); // copy the items
            auto cst = conj->cst ? scp.get_core().arith_value(static_cast<arith_term &>(*conj->cst->evaluate(scp, ctx))).get_rational() : utils::rational::one;
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
                env cctx(scp.get_core(), ctx);
                cctx.items.emplace(enum_id.id, inst);
                for (auto &stmt : stmts)
                    stmt->execute(scp, cctx);
            }
        else
            throw std::runtime_error("Invalid type reference");
    }

    void return_statement::execute(const scope &scp, env &ctx) const
    { // return from a method
        ctx.items.emplace(return_kw, xpr->evaluate(scp, ctx));
    }

    void formula_statement::execute(const scope &scp, env &ctx) const
    { // create a new atom
        std::map<std::string, expr, std::less<>> c_args;
        for (auto &[id, expr] : args)
            c_args.emplace(id.id, expr->evaluate(scp, ctx));

        if (!tau.empty())
        {
            auto c_tau = ctx.get(tau[0].id);
            for (size_t i = 1; i < tau.size(); ++i)
                if (auto ct = dynamic_cast<component *>(c_tau.get()))
                    c_tau = ct->get(tau[i].id);
                else if (auto c = dynamic_cast<enum_term *>(c_tau.get()))
                    c_tau = c->get(tau[i].id);
                else if (auto c = dynamic_cast<atom_term *>(c_tau.get()))
                    c_tau = c->get(tau[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            c_args.emplace(tau_kw, c_tau);
        }
        else
            try
            {
                c_args.emplace(tau_kw, ctx.get(tau_kw));
            }
            catch (const std::exception &)
            { // there is no tau..
            }

        auto &pred = tau.empty() ? scp.get_predicate(predicate_name.id) : static_cast<component_type &>(c_args.at(tau_kw).get()->get_type()).get_predicate(predicate_name.id);

        // we initialize the unassigned atom's fields..
        std::queue<predicate *> q;
        q.push(&pred);
        while (!q.empty())
        {
            auto p = q.front();
            for (const auto &[name, f] : p->get_fields())
                if (c_args.find(name) == c_args.end())
                { // the field is unassigned
                    auto &tp = f->get_type();
                    if (tp.is_primitive())
                        c_args.emplace(name, tp.new_instance());
                    else if (auto ct = dynamic_cast<component_type *>(&tp))
                        switch (ct->get_instances().size())
                        {
                        case 0: // no instances
                            throw inconsistency_exception();
                        case 1: // only one instance
                            c_args.emplace(name, *ct->get_instances().begin());
                            break;
                        default:
                        { // multiple instances
                            std::vector<std::reference_wrapper<utils::enum_val>> values;
                            for (auto &inst : ct->get_instances())
                                values.emplace_back(*inst);
                            c_args.emplace(name, ctx.get_core().new_enum(*ct, std::move(values)));
                        }
                        }
                    else
                        throw std::runtime_error("Invalid type reference");
                }
            q.pop();
            for (auto &parent : p->get_parents())
                q.push(&parent.get());
        }

        auto atm = scp.get_core().new_atom(is_fact, pred, std::move(c_args));

        ctx.items.emplace(id.id, std::move(atm));
    }
} // namespace riddle
