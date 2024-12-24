#include "statement.hpp"
#include "core.hpp"

namespace riddle
{
    void local_field_statement::execute(const scope &scp, env &ctx) const
    {
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
                }
    }
} // namespace riddle
