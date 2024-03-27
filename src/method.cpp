#include "method.hpp"
#include "type.hpp"
#include "statement.hpp"
#include "core.hpp"

namespace riddle
{
    method::method(scope &parent, std::optional<std::reference_wrapper<type>> return_type, const std::string &name, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body) : scope(parent.get_core(), parent), return_type(return_type), name(name), body(body)
    {
        arguments.reserve(args.size());
        for (auto &arg : args)
        {
            arguments.push_back(*arg);
            add_field(std::move(arg));
        }
        if (!is_core(parent))
            add_field(std::make_unique<field>(static_cast<component_type &>(get_parent()), "this", std::vector<std::unique_ptr<expression>>{}, true));
    }

    std::shared_ptr<item> method::invoke(std::shared_ptr<env> &ctx, std::vector<std::shared_ptr<item>> &&args)
    {
        if (args.size() != arguments.size())
            throw std::runtime_error("Invalid number of arguments.");

        // we create a new environment for the method
        auto c_env = std::make_shared<env>(get_core(), ctx);

        // we add the arguments to the environment
        for (size_t i = 0; i < args.size(); i++)
        {
            if (!arguments.at(i).get().get_type().is_assignable_from(args.at(i)->get_type()))
                throw std::runtime_error("Invalid argument type: " + arguments.at(i).get().get_type().get_name() + " is not assignable from " + args.at(i)->get_type().get_name() + ".");
            c_env->items.emplace(arguments.at(i).get().get_name(), args.at(i));
        }

        // we execute the method body
        for (const auto &stmt : body)
            stmt->execute(*this, c_env);

        // we return the result
        if (return_type)
            return c_env->items.at("return");
        else
            return nullptr;
    }
} // namespace riddle