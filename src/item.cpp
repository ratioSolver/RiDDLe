#include "core.hpp"

namespace riddle
{
    bool_item::bool_item(bool_type &tp) : item(tp) {}
    arith_item::arith_item(int_type &tp) : item(tp) {}
    arith_item::arith_item(real_type &tp) : item(tp) {}
    arith_item::arith_item(time_type &tp) : item(tp) {}
    string_item::string_item(string_type &tp) : item(tp) {}

    enum_item::enum_item(type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) : item(tp), values(std::move(values)) {}

    component::component(component_type &t) : item(t), env(t.get_core(), t.get_core()) {}

    std::shared_ptr<bool_item> component::operator==(std::shared_ptr<item> rhs) const { return get_core().new_bool(this == rhs.get()); }

    atom::atom(predicate &t, bool fact, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args) : item(t), env(t.get_core(), atom_parent(t, args), std::move(args)), fact(fact) {}

    env &atom::atom_parent(const predicate &t, const std::map<std::string, std::shared_ptr<item>, std::less<>> &args)
    {
        if (args.count(tau_kw))
            return *static_cast<component *>(args.at(tau_kw).get());
        else
            return t.get_core();
    }
} // namespace riddle