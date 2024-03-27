#include "item.hpp"
#include "type.hpp"
#include "core.hpp"

namespace riddle
{
    item::item(type &t) : tp(t) {}

    bool_item::bool_item(bool_type &t, const utils::lit &l) : item(t), value(l) {}

    arith_item::arith_item(int_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(real_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(time_type &t, const utils::lin &l) : item(t), value(l) {}

    string_item::string_item(string_type &t, const std::string &s) : item(t), value(s) {}

    enum_item::enum_item(type &t, VARIABLE_TYPE v) : item(t), env(t.get_scope().get_core()), value(v) {}
    std::shared_ptr<item> enum_item::get(const std::string &name) { return get_core().get(*this, name); }

    component::component(type &t, std::shared_ptr<env> parent) : item(t), env(t.get_scope().get_core(), parent) {}

    atom::atom(predicate &t, bool is_fact, const utils::lit &sigma, std::map<std::string, std::shared_ptr<item>> &&args) : item(t), env(t.get_scope().get_core(), nullptr, std::move(args)), fact(is_fact), sigma(sigma) {}
} // namespace riddle