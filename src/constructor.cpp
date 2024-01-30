#include <stdexcept>
#include <algorithm>
#include "constructor.hpp"
#include "type.hpp"
#include "item.hpp"
#include "declaration.hpp"

namespace riddle
{
    constructor::constructor(std::shared_ptr<scope> parent, std::vector<std::unique_ptr<field>> &&args, std::vector<init_element> &inits, std::vector<std::unique_ptr<statement>> &stmts) : scope(parent->get_core(), parent), args(std::move(args)), inits(inits), stmts(stmts) {}

    std::shared_ptr<item> constructor::invoke(std::vector<std::shared_ptr<item>> &&arguments)
    {
        if (arguments.size() != args.size())
            throw std::runtime_error("Invalid number of arguments.");
        for (size_t i = 0; i < args.size(); i++)
            if (args.at(i)->get_type().is_assignable_from(arguments.at(i)->get_type()))
                throw std::runtime_error("Invalid argument type: " + args.at(i)->get_type().get_name() + " is not assignable from " + arguments.at(i)->get_type().get_name() + ".");

        auto &tp = *std::static_pointer_cast<component_type>(get_parent());
        // we create a new instance of the type
        auto instance = std::static_pointer_cast<component>(tp.new_instance());
        // we create a new environment for the constructor
        env ctx(get_core(), instance);
        for (size_t i = 0; i < args.size(); i++)
            ctx.items.emplace(args.at(i)->get_name(), arguments.at(i));

        // we initialize the instance
        for (auto &init : inits)
        {
            auto f = get_field(init.get_name().id);
            if (f) // the field is declared in the component type
            {
                if (f->get().get_type().is_primitive()) // we initialize a primitive field
                    instance->items.emplace(init.get_name().id, init.get_args().at(0)->evaluate(*this, ctx));
                else // we initialize a component field by invoking its constructor
                {
                    auto tp = dynamic_cast<component_type *>(&f->get().get_type());
                    if (!tp)
                        throw std::runtime_error("Cannot invoke constructor on non-component type " + f->get().get_type().get_name());

                    std::vector<std::reference_wrapper<const type>> arg_types;
                    std::vector<std::shared_ptr<item>> arguments;

                    for (auto &arg : init.get_args())
                    {
                        auto xpr = arg->evaluate(*this, ctx);
                        arg_types.push_back(xpr->get_type());
                        arguments.push_back(xpr);
                    }

                    instance->items.emplace(init.get_name().id, tp->get_constructor(arg_types).invoke(std::move(arguments)));
                }
            }
            else if (auto st = std::find_if(tp.get_parents().begin(), tp.get_parents().end(), [&init](const std::shared_ptr<component_type> &tp)
                                            { return tp->get_name() == init.get_name().id; });
                     st != tp.get_parents().end())
            { // we invoke a supertype constructor
                std::vector<std::reference_wrapper<const type>> arg_types;
                std::vector<std::shared_ptr<item>> arguments;

                for (auto &arg : init.get_args())
                {
                    auto xpr = arg->evaluate(*this, ctx);
                    arg_types.push_back(xpr->get_type());
                    arguments.push_back(xpr);
                }

                (*st)->get_constructor(arg_types).invoke(std::move(arguments));
            }
            else
                throw std::runtime_error("Cannot find supertype " + init.get_name().id + ".");
        }

        // we initialize the uninitialized fields
        for (const auto &[f_name, f] : tp.get_fields())
            if (!f->is_synthetic() && instance->items.find(f_name) == instance->items.end())
            { // the field is not initialized
                if (f->get_init())
                    instance->items.emplace(f_name, f->get_init()->evaluate(*this, ctx));
                else if (auto c_tp = dynamic_cast<component_type *>(&f->get_type()))
                    instance->items.emplace(f_name, f->get_type().new_instance());
                else
                    instance->items.emplace(f_name, f->get_type().new_instance());
            }

        // we execute the constructor statements
        for (auto &stmt : stmts)
            stmt->execute(*this, ctx);

        return instance;
    }
} // namespace riddle