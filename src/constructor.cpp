#include "core.hpp"
#include "exceptions.hpp"
#include <algorithm>

namespace riddle
{
    constructor::constructor(scope &scp, std::vector<utils::u_ptr<field>> &&args, const std::vector<std::pair<id_token, std::vector<utils::u_ptr<expression>>>> &inits, const std::vector<utils::u_ptr<statement>> &body) noexcept : scope(scp.get_core(), scp), inits(inits), body(body)
    {
        for (auto &arg : args)
        {
            this->args.emplace_back(arg->get_name());
            add_field(std::move(arg));
        }
    }

    void constructor::invoke(utils::s_ptr<component> self, std::vector<expr> &&args) const
    {
        if (args.size() != this->args.size())
            throw std::invalid_argument("invalid number of arguments");
        for (size_t i = 0; i < args.size(); ++i)
            if (!args[i]->get_type().is_assignable_from(get_field(this->args[i]).get_type()))
                throw std::invalid_argument("invalid argument type");

        auto &tp = static_cast<component_type &>(get_parent());
        env ctx(get_core(), *self); // the context in which the constructor is invoked..
        for (size_t i = 0; i < args.size(); ++i)
            ctx.items.emplace(this->args[i], args[i]);

        // we initialize the instance
        for (const auto &init : inits)
            if (auto f = tp.fields.find(init.first.id); f != tp.fields.end())
            {
                if (init.second.empty()) // we initialize the field with a new instance
                    self->items.emplace(f->first, f->second->get_type().new_instance());
                else
                {
                    std::vector<expr> init_args;
                    std::vector<utils::ref_wrapper<const type>> argument_types;

                    for (const auto &arg : init.second)
                        init_args.emplace_back(arg->evaluate(*this, ctx));
                    for (const auto &arg : init_args)
                        argument_types.emplace_back(arg->get_type());

                    if (init_args.size() == 1 && argument_types[0]->is_assignable_from(f->second->get_type()))
                        self->items.emplace(f->first, init_args[0]); // we assign the argument to the field
                    else
                    { // we invoke the constructor of the field
                        auto &ctp = static_cast<component_type &>(f->second->get_type());
                        auto instance = utils::s_ptr_cast<component>(ctp.new_instance());
                        ctp.get_constructor(argument_types).invoke(instance, std::move(init_args));
                        self->items.emplace(f->first, std::move(instance));
                    }
                }
            }
            else
            {
                if (auto st = std::find_if(tp.get_parents().begin(), tp.get_parents().end(), [&init](const auto &parent)
                                           { return parent->get_name() == init.first.id; });
                    st != tp.get_parents().end())
                { // we invoke a supertype constructor
                    std::vector<expr> init_args;
                    std::vector<utils::ref_wrapper<const type>> argument_types;

                    for (const auto &arg : init.second)
                        init_args.emplace_back(arg->evaluate(*this, ctx));
                    for (const auto &arg : init_args)
                        argument_types.emplace_back(arg->get_type());

                    (*st)->get_constructor(argument_types).invoke(self, std::move(init_args));
                }
            }

        // we initialize the uninitialized fields
        for (const auto &[name, f] : tp.fields)
            if (!f->is_synthetic() && self->items.find(name) == self->items.end()) // the field is not initialized
            {
                if (f->get_expression())
                { // initialize with an expression
                    auto val = f->get_expression()->evaluate(*this, ctx);
                    if (f->get_type().is_assignable_from(val->get_type()))
                        self->items.emplace(name, val);
                    else
                        throw std::runtime_error("Invalid assignment");
                }
                else if (f->get_type().is_primitive()) // initialize with a default value
                    self->items.emplace(name, f->get_type().new_instance());
                else if (auto ct = dynamic_cast<component_type *>(&f->get_type()))
                    switch (ct->get_instances().size())
                    {
                    case 0: // no instances
                        throw inconsistency_exception();
                    case 1: // only one instance
                        self->items.emplace(name, *ct->get_instances().begin());
                        break;
                    default:
                    { // multiple instances
                        std::vector<utils::ref_wrapper<utils::enum_val>> values;
                        for (auto &inst : ct->get_instances())
                            values.emplace_back(*inst);
                        self->items.emplace(name, get_core().new_enum(*ct, std::move(values)));
                    }
                    }
                else
                    throw std::runtime_error("Invalid type reference");
            }

        // we execute the constructor statements
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);
    }
} // namespace riddle
