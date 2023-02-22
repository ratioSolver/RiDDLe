#include "item.h"
#include "type.h"
#include "core.h"

namespace riddle
{
    item::item(type &tp) : tp(tp) {}

    core &item::get_core() { return tp.get_core(); }
    const core &item::get_core() const { return tp.get_core(); }

    complex_item::complex_item(type &tp) : item(tp) {}

    RIDDLE_EXPORT expr &complex_item::get(const std::string &name)
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }

    enum_item::enum_item(type &tp) : item(tp) {}

    RIDDLE_EXPORT expr &enum_item::get(const std::string &name)
    {
        const auto it = items.lower_bound(name);
        if (it != items.end() && it->first == name)
            return it->second;
        else
        { // we generate a new variable..
            expr c_e(this);
            auto e = get_core().get_enum(c_e, name);
            items.emplace_hint(it, name, e);
            return items.at(name);
        }
    }
} // namespace riddle