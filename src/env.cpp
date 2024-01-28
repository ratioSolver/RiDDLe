#include "env.hpp"
#include "core.hpp"

namespace riddle
{
    env::env(core &c, std::shared_ptr<env> parent) : cr(c), parent(parent) {}

    std::shared_ptr<item> env::get(const std::string &name) const
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else if (parent != nullptr)
            return parent->get(name);
        else
            return cr.get(name);
    }
} // namespace riddle
