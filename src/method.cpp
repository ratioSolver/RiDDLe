#include "method.hpp"

namespace riddle
{
    method::method(scope &scp, std::vector<std::unique_ptr<field>> &&args, std::vector<std::unique_ptr<statement>> &&body) noexcept : scope(scp.get_core(), scp), body(std::move(body))
    {
        for (auto &arg : args)
        {
            this->args.emplace_back(arg->get_name());
            add_field(std::move(arg));
        }
    }

    std::shared_ptr<item> method::invoke(env &ctx, std::vector<std::shared_ptr<item>> &&args) const
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
