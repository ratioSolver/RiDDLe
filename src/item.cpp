#include "item.h"
#include "type.h"

namespace riddle
{
    item::item(type &tp) : tp(tp) {}

    complex_item::complex_item(type &tp) : item(tp) {}

    RIDDLE_EXPORT expr &complex_item::get(const std::string &name)
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }
} // namespace riddle