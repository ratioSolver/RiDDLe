#include "item.hpp"
#include "type.hpp"
#include <type_traits>

namespace riddle
{
    bool_item::bool_item(bool_type &tp, const utils::lit &l) : item(tp), value(l) {}
    arith_item::arith_item(int_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(real_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(time_type &t, const utils::lin &l) : item(t), value(l) {}
    string_item::string_item(string_type &tp, std::string &&s) : item(tp), value(std::move(s)) {}

    enum_item::enum_item(enum_type &tp, utils::var v) : item(tp), value(v) {}

    component::component(component_type &t, env &parent) : item(t), env(t.get_core(), parent) {}
} // namespace riddle