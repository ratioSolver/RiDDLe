#include "constructor.hpp"
#include "type.hpp"

namespace riddle
{
    constructor::constructor(scope &scp, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body) noexcept : scope(scp.get_core(), scp), body(body)
    {
        for (auto &arg : args)
        {
            this->args.emplace_back(arg->get_name());
            add_field(std::move(arg));
        }
    }

    std::shared_ptr<item> constructor::invoke(std::vector<std::shared_ptr<item>> &&args) const
    {
        if (args.size() != this->args.size())
            throw std::invalid_argument("invalid number of arguments");
        for (size_t i = 0; i < args.size(); ++i)
            if (!args[i]->get_type().is_assignable_from(get_field(this->args[i]).get_type()))
                throw std::invalid_argument("invalid argument type");

        auto &tp = static_cast<component_type &>(get_parent());                 // the type of the component..
        auto instance = std::static_pointer_cast<component>(tp.new_instance()); // the new instance of the component..

        env ctx(get_core(), *instance); // the context in which the constructor is invoked..
        for (size_t i = 0; i < args.size(); ++i)
            ctx.items.emplace(this->args[i], args[i]);

        throw std::runtime_error("not implemented");

        return instance; // return the new instance of the component..
    }
} // namespace riddle
