#include "core.hpp"
#include <type_traits>

namespace riddle
{
    bool_item::bool_item(bool_type &tp, const utils::lit &l) : item(tp), value(l) {}
    arith_item::arith_item(int_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(real_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(time_type &t, const utils::lin &l) : item(t), value(l) {}
    string_item::string_item(string_type &tp, std::string &&s) : item(tp), value(std::move(s)) {}

    enum_item::enum_item(type &tp, utils::var v) : item(tp), value(v) {}

    component::component(component_type &t) : item(t), env(t.get_core(), t.get_core()) {}

    env &atom_parent(const predicate &t, const std::map<std::string, std::shared_ptr<item>, std::less<>> &args)
    {
        if (args.count(tau_kw))
            return *static_cast<component *>(args.at(tau_kw).get());
        else
            return t.get_core();
    }

    atom::atom(predicate &t, bool fact, const utils::lit &sigma, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args) : item(t), env(t.get_core(), atom_parent(t, args), std::move(args)), fact(fact), sigma(sigma) {}
} // namespace riddle