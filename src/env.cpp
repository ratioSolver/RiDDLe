#include "env.hpp"
#include "item.hpp"
#include "core.hpp"

namespace riddle
{
    env::env(core &c, env &parent, std::map<std::string, std::unique_ptr<item>, std::less<>> &&items) noexcept : cr(c), parent(parent), items(std::move(items)) {}

    std::optional<std::reference_wrapper<item>> env::get(std::string_view name) noexcept
    {
        if (auto it = items.find(name); it != items.end())
            return *it->second;
        else
            return parent.get(name);
    }
} // namespace riddle