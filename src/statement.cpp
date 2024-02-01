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
                ctx->items.emplace(field.get_id().id, field.get_expression()->evaluate(*scp, *ctx));
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
        static_cast<env &>(*c_env).items.emplace(field_name.id, rhs->evaluate(*scp, *ctx));
    }

    void expression_statement::execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const { scp->get_core().assert_fact(expr->evaluate(*scp, *ctx)); }

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
} // namespace riddle
