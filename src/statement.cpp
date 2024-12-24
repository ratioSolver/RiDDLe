#include "core.hpp"
#include "parser.hpp"

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
} // namespace riddle
