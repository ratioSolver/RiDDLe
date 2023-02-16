#include "item.h"

namespace riddle
{
    item::item(env &e, type &tp) : env(e), tp(tp) {}

    complex_item::complex_item(env &e, type &tp) : item(e, tp) {}

    expr complex_item::get(const std::string &name) const
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        return env::get(name);
    }
} // namespace riddle