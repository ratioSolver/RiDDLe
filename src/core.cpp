#include "core.hpp"

namespace riddle
{
    core::core() : scope(*this), env(*this) {}

    std::optional<field> core::get_field(const std::string &name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return std::optional<field>(*it->second.get());
        else
            return std::nullopt;
    }

    std::shared_ptr<item> core::get(const std::string &name) const
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else
            return nullptr;
    }
} // namespace riddle