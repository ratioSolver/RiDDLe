#include <stdexcept>
#include <algorithm>
#include "constructor.hpp"
#include "type.hpp"
#include "item.hpp"
#include "declaration.hpp"

namespace riddle
{
    constructor::constructor(scope &parent, std::vector<std::unique_ptr<field>> &&ctr_args, const std::vector<init_element> &inits, const std::vector<std::unique_ptr<statement>> &body) : scope(parent.get_core(), parent), inits(inits), body(body)
    {
        args.reserve(ctr_args.size());
        for (auto &arg : ctr_args)
        {
            args.push_back(*arg);
            add_field(std::move(arg));
        }
        add_field(std::make_unique<field>(static_cast<component_type &>(get_parent()), "this", std::vector<std::unique_ptr<expression>>{}, true));
    }

    std::shared_ptr<item> constructor::invoke(std::vector<std::shared_ptr<item>> &&arguments)
    {
        if (arguments.size() != args.size())
            throw std::runtime_error("Invalid number of arguments.");
        for (size_t i = 0; i < args.size(); i++)
            if (!args.at(i).get().get_type().is_assignable_from(arguments.at(i)->get_type()))
                throw std::runtime_error("Invalid argument type: " + args.at(i).get().get_type().get_name() + " is not assignable from " + arguments.at(i)->get_type().get_name() + ".");

        auto &tp = static_cast<component_type &>(get_parent());
        // we create a new instance of the type
        auto instance = std::static_pointer_cast<component>(tp.new_instance());
        // we create a new environment for the constructor
        auto ctx = std::make_shared<env>(get_core(), instance);
        for (size_t i = 0; i < args.size(); i++)
            ctx->items.emplace(args.at(i).get().get_name(), arguments.at(i));

        // we initialize the instance
        for (const auto &init : inits)
            if (auto f = get_field(init.get_name().id)) // the field is declared in the component type
            {
                if (f->get().get_type().is_primitive()) // we initialize a primitive field
                    instance->items.emplace(init.get_name().id, init.get_args().at(0)->evaluate(*this, ctx));
                else // we initialize a component field by invoking its constructor
                {
                    auto c_tp = dynamic_cast<component_type *>(&f->get().get_type());
                    if (!c_tp)
                        throw std::runtime_error("Cannot invoke constructor on non-component type " + f->get().get_type().get_name());

                    std::vector<std::reference_wrapper<const type>> arg_types;
                    std::vector<std::shared_ptr<item>> args;

                    for (const auto &arg : init.get_args())
                    {
                        auto xpr = arg->evaluate(*this, ctx);
                        arg_types.push_back(xpr->get_type());
                        args.push_back(xpr);
                    }

                    if (arg_types.size() == 1 && f->get().get_type().is_assignable_from(arg_types.at(0)))
                        instance->items.emplace(init.get_name().id, args.at(0)); // we assign the argument to the field
                    else if (auto c = c_tp->get_constructor(arg_types))
                        instance->items.emplace(init.get_name().id, c->get().invoke(std::move(args))); // we invoke the constructor and assign the result to the field
                    else
                        throw std::runtime_error("Cannot find constructor for class " + init.get_name().id);
                }
            }
            else if (auto st = std::find_if(tp.get_parents().begin(), tp.get_parents().end(), [&init](const std::reference_wrapper<component_type> &tp)
                                            { return tp.get().get_name() == init.get_name().id; });
                     st != tp.get_parents().end())
            { // we invoke a supertype constructor
                std::vector<std::reference_wrapper<const type>> arg_types;
                std::vector<std::shared_ptr<item>> args;

                for (const auto &arg : init.get_args())
                {
                    auto xpr = arg->evaluate(*this, ctx);
                    arg_types.push_back(xpr->get_type());
                    args.push_back(xpr);
                }

                if (auto c = (*st).get().get_constructor(arg_types))
                    instance->items.emplace(init.get_name().id, c->get().invoke(std::move(args)));
                else
                    throw std::runtime_error("Cannot find supertype " + init.get_name().id + ".");
            }
            else
                throw std::runtime_error("Cannot find supertype " + init.get_name().id + ".");

        // we initialize the uninitialized fields
        for (const auto &[f_name, f] : tp.get_fields())
            if (!f->is_synthetic() && instance->items.find(f_name) == instance->items.end()) // the field is not initialized
                switch (f->get_inits().size())
                {
                case 0:
                    if (auto c_tp = dynamic_cast<component_type *>(&f->get_type()))
                        instance->items.emplace(f_name, c_tp->new_instance());
                    else
                        instance->items.emplace(f_name, f->get_type().new_instance());
                    break;
                case 1:
                    if (f->get_inits().size() == 1)
                    {
                        auto xpr = f->get_inits().at(0)->evaluate(*this, ctx);
                        if (f->get_type().is_assignable_from(xpr->get_type()))
                            instance->items.emplace(f_name, xpr);
                        else if (auto c_tp = dynamic_cast<component_type *>(&f->get_type()))
                        {
                            if (auto c = c_tp->get_constructor({xpr->get_type()}))
                                instance->items.emplace(f_name, c->get().invoke({xpr}));
                            else
                                throw std::runtime_error("Cannot find constructor for class " + f->get_type().get_name());
                        }
                    }
                    break;
                default:
                {
                    std::vector<std::reference_wrapper<const type>> arg_types;
                    std::vector<std::shared_ptr<item>> arguments;

                    for (const auto &arg : f->get_inits())
                    {
                        auto xpr = arg->evaluate(*this, ctx);
                        arg_types.push_back(xpr->get_type());
                        arguments.push_back(xpr);
                    }

                    if (auto c_tp = dynamic_cast<component_type *>(&f->get_type()))
                    {
                        if (auto c = c_tp->get_constructor(arg_types))
                            instance->items.emplace(f_name, c->get().invoke(std::move(arguments)));
                        else
                            throw std::runtime_error("Cannot find constructor for class " + f->get_type().get_name());
                    }
                }
                }

        // we execute the constructor statements
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);

        return instance;
    }
} // namespace riddle