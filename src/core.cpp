#include "core.h"
#include "item.h"
#include <stdexcept>

namespace riddle
{
    RIDDLE_EXPORT core::core() : scope(*this), env(*this) {}

    RIDDLE_EXPORT expr core::get(const std::string &name) const
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }
} // namespace riddle