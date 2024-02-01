#include "statement.hpp"
#include "type.hpp"
#include "core.hpp"

namespace riddle
{
    void local_field_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        auto tp_opt = scp->get_type(field_type.front().id);
        if (!tp_opt)
            throw std::runtime_error("Cannot find class " + field_type.front().id);
        auto tp = &tp_opt.value().get();
        if (!tp)
            throw std::runtime_error("Class " + field_type.front().id + " is not a component type");
        for (auto it = field_type.begin() + 1; it != field_type.end(); it++)
            if (auto cmp_tp = dynamic_cast<component_type *>(tp))
            {
                tp_opt = cmp_tp->get_type(it->id);
                if (!tp_opt)
                    throw std::runtime_error("Cannot find class " + it->id);
                tp = &tp_opt.value().get();
            }

        for (const auto &field : fields)
            if (tp->is_primitive())
                ctx->items.emplace(field.get_id().id, field.get_expression()->evaluate(*scp, ctx));
            else if (auto ct = dynamic_cast<component_type *>(tp))
                switch (ct->get_instances().size())
                {
                case 0:
                    throw inconsistency_exception();
                case 1:
                    ctx->items.emplace(field.get_id().id, ct->get_instances().front());
                default:
                    ctx->items.emplace(field.get_id().id, scp->get_core().new_enum(*ct, ct->get_instances()));
                }
            else if (auto et = dynamic_cast<enum_type *>(tp))
            {
                auto values = et->get_values();
                switch (values.size())
                {
                case 0:
                    throw inconsistency_exception();
                case 1:
                    ctx->items.emplace(field.get_id().id, values.front());
                default:
                    ctx->items.emplace(field.get_id().id, scp->get_core().new_enum(*et, values));
                }
            }
            else if (auto td = dynamic_cast<typedef_type *>(tp))
                ctx->items.emplace(field.get_id().id, td->new_instance());
            else
                throw std::runtime_error("Cannot create instance of type " + tp->get_name());
    }

    void assignment_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        auto c_env = ctx;
        for (const auto &id : object_id)
        {
            auto itm = c_env->get(id.id);
            if (!itm)
                throw std::runtime_error("Cannot find object " + id.id);
            if (auto cmp = std::dynamic_pointer_cast<env>(itm))
                c_env = cmp;
            else
                throw std::runtime_error("Object " + id.id + " is not an environment");
        }
        static_cast<env &>(*c_env).items.emplace(field_name.id, rhs->evaluate(*scp, ctx));
    }

    void expression_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const { scp->get_core().assert_fact(expr->evaluate(*scp, ctx)); }

    void disjunction_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        std::vector<std::unique_ptr<conjunction>> conjs;
        for (const auto &block : blocks)
            conjs.push_back(std::make_unique<conjunction>(scp, ctx, *block));
        scp->get_core().new_disjunction(std::move(conjs));
    }

    void for_all_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        auto tp_opt = scp->get_type(enum_type.front().id);
        if (!tp_opt)
            throw std::runtime_error("Cannot find class " + enum_type.front().id);
        auto tp = dynamic_cast<component_type *>(&tp_opt.value().get());
        if (!tp)
            throw std::runtime_error("Class " + enum_type.front().id + " is not a component type");
        for (auto it = enum_type.begin() + 1; it != enum_type.end(); it++)
        {
            tp_opt = tp->get_type(it->id);
            if (!tp_opt)
                throw std::runtime_error("Cannot find class " + it->id);
            tp = dynamic_cast<component_type *>(&tp_opt.value().get());
            if (!tp)
                throw std::runtime_error("Class " + it->id + " is not a component type");
        }

        for (auto instance : tp->get_instances())
        {
            auto c_env = std::make_shared<env>(scp->get_core(), ctx);
            c_env->items.emplace(enum_id.id, instance);
            for (const auto &stmn : statements)
                stmn->execute(scp, c_env);
        }
    }

    void return_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        if (expr)
            ctx->items.emplace("return", expr->evaluate(*scp, ctx));
    }

    void formula_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const
    {
        auto c_env = ctx;
        for (const auto &id : formula_scope)
        {
            auto itm = c_env->get(id.id);
            if (!itm)
                throw std::runtime_error("Cannot find object " + id.id);
            if (auto cmp = std::dynamic_pointer_cast<component>(itm))
                c_env = cmp;
            else
                throw std::runtime_error("Object " + id.id + " is not a component");
        }

        std::optional<std::reference_wrapper<predicate>> pred_opt;
        if (c_env) // the formula's scope is explicitely declared..
            pred_opt = static_cast<component_type &>(static_cast<component &>(*c_env).get_type()).get_predicate(predicate_name.id);
        else // ..or it is implicitely declared
            pred_opt = scp->get_predicate(predicate_name.id);

        if (!pred_opt)
            throw std::runtime_error("Cannot find predicate " + predicate_name.id);

        auto &pred = pred_opt.value().get();

        std::map<std::string, std::shared_ptr<item>> args;
        for (const auto &arg : arguments)
        {
            auto tp_opt = pred.get_field(arg.get_id().id);
            if (!tp_opt)
                throw std::runtime_error("Cannot find field " + arg.get_id().id);
            auto &tp = tp_opt.value().get().get_type();
            auto val = arg.get_expression()->evaluate(*scp, ctx);
            if (tp.is_assignable_from(val->get_type())) // the target type is a superclass of the assignment..
                args.emplace(arg.get_id().id, val);
            else if (val->get_type().is_assignable_from(tp)) // ..or the assignment is a superclass of the target type
            {
                
            }
            else
                throw std::runtime_error("Cannot assign " + val->get_type().get_name() + " to " + tp.get_name());
        }
    }
} // namespace riddle
