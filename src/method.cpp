#include "method.hpp"

namespace riddle
{
    method::method(scope &scp, std::optional<std::reference_wrapper<type>> return_type, std::string_view name, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body) noexcept : scope(scp.get_core(), scp), return_type(return_type), name(name), body(body)
    {
        for (auto &arg : args)
        {
            this->args.emplace_back(arg->get_name());
            add_field(std::move(arg));
        }
    }

    expr method::invoke(std::shared_ptr<component> self, std::vector<expr> &&args) const
    {
        // the context in which the method is invoked..
        env ctx(get_core(), *self);
        if (self)
            ctx.items.emplace(this_kw, self); // the current instance
        for (size_t i = 0; i < this->args.size(); ++i)
            ctx.items.emplace(this->args[i], args[i]); // the arguments

        // we execute the body of the method
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);

        if (auto it = ctx.items.find("return"); it != ctx.items.end())
            return it->second;
        return nullptr;
    }
} // namespace riddle
