#include "env.hpp"
#include "core.hpp"

namespace riddle
{
    env::env(core &c, std::shared_ptr<env> parent, std::map<std::string, std::shared_ptr<item>> &&items) : cr(c), parent(parent), items(std::move(items)) {}

    std::shared_ptr<item> env::get(const std::string &name) const noexcept
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else if (parent != nullptr)
            return parent->get(name);
        else
            return cr.get(name);
    }
} // namespace riddle
