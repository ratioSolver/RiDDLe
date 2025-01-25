#include "method.hpp"

namespace riddle
{
    method::method(scope &scp, std::optional<utils::ref_wrapper<type>> return_type, std::string_view name, std::vector<utils::u_ptr<field>> &&args, const std::vector<utils::u_ptr<statement>> &body) noexcept : scope(scp.get_core(), scp), return_type(return_type), name(name), body(body)
    {
        for (auto &arg : args)
        {
            this->args.emplace_back(arg->get_name());
            add_field(std::move(arg));
        }
    }

    expr method::invoke(env &ctx, std::vector<expr> &&args) const
    {
        env local(ctx.get_core(), ctx);

        for (size_t i = 0; i < this->args.size(); ++i)
            local.items.emplace(this->args[i], args[i]);

        for (const auto &stmt : body)
            stmt->execute(*this, local);

        if (auto it = local.items.find("return"); it != local.items.end())
            return it->second;
        return nullptr;
    }
} // namespace riddle
