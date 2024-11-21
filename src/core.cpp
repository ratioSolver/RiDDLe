#include "core.hpp"
#include "item.hpp"

namespace riddle
{
    core::core() noexcept : env(*this, *this) {}

    std::optional<std::reference_wrapper<item>> core::get(std::string_view name) noexcept
    {
        if (auto it = items.find(name); it != items.end())
            return *it->second;
        return std::nullopt;
    }
} // namespace riddle