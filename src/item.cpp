#include "item.h"
#include "type.h"
#include "core.h"

namespace riddle
{
    RIDDLE_EXPORT core &item::get_core() { return tp.get_core(); }
    RIDDLE_EXPORT const core &item::get_core() const { return tp.get_core(); }

    RIDDLE_EXPORT complex_item::complex_item(type &tp) : item(tp), env(&tp.get_core()) {}

    enum_item::enum_item(type &tp) : item(tp), env(&tp.get_core()) {}

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

    atom::atom(predicate &p, bool is_fact) : complex_item(p), is_fact_(is_fact) {}
} // namespace riddle