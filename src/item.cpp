#include "core.hpp"
#include <cassert>

namespace riddle
{
    bool_item::bool_item(bool_type &tp) : item(tp) {}
    json::json bool_item::to_json() const
    {
        json::json j_val{{"type", std::string_view(get_type().get_name())}}; // we add the type of the item..
        switch (get_type().get_scope().get_core().bool_value(*this))
        {
        case utils::True:
            j_val["val"] = "True";
            break;
        case utils::False:
            j_val["val"] = "False";
            break;
        default:
            j_val["val"] = "Undefined";
            break;
        }
        return j_val;
    }

    arith_item::arith_item(int_type &tp) : item(tp) {}
    arith_item::arith_item(real_type &tp) : item(tp) {}
    arith_item::arith_item(time_type &tp) : item(tp) {}
    json::json arith_item::to_json() const
    {
        json::json j_val{{"type", std::string_view(get_type().get_name())}}; // we add the type of the item..
        const auto val = get_type().get_scope().get_core().arith_value(*this);
        if (get_type().get_name() == "int")
        {
            assert(is_integer(val));
            j_val["val"] = static_cast<int64_t>(val.get_rational().numerator());
        }
        else
        {
            j_val["num"] = static_cast<int64_t>(val.get_rational().numerator());
            j_val["den"] = static_cast<int64_t>(val.get_rational().denominator());
            if (val.get_infinitesimal() != utils::rational::zero)
                j_val["inf"] = {{"num", static_cast<int64_t>(val.get_infinitesimal().numerator())},
                                {"den", static_cast<int64_t>(val.get_infinitesimal().denominator())}};
        }
        return j_val;
    }

    string_item::string_item(string_type &tp) : item(tp) {}
    json::json string_item::to_json() const { return {{"type", std::string_view(get_type().get_name()), {"val", get_type().get_scope().get_core().string_value(*this)}}}; }

    enum_item::enum_item(type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) : item(tp), values(std::move(values)) {}
    json::json enum_item::to_json() const
    {
        json::json j_val{{"type", "enum"}}; // we add the type of the enum item..

        // we add the full name of the type of the enum item..
        std::string tp_name = get_type().get_name();
        const auto *t = &get_type();
        while (const auto *et = dynamic_cast<const type *>(&t->get_scope()))
        {
            tp_name.insert(0, et->get_name() + ".");
            t = et;
        }
        j_val["enums_type"] = tp_name;

        // we add the domain of the enum item..
        auto vals = get_type().get_scope().get_core().enum_value(*this);
        json::json j_vals(json::json_type::array);
        for (const auto &val : vals)
            j_vals.push_back(static_cast<item &>(val.get()).get_id());
        j_val["vals"] = std::move(j_vals);

        return j_val;
    }

    component::component(component_type &t) : item(t), env(t.get_core(), t.get_core()) {}
    json::json component::to_json() const
    {
        json::json j_itm{{"type", get_type().get_full_name()}}; // we add the type of the item..
#ifdef COMPUTE_NAMES
        j_itm["name"] = std::string_view(get_type().get_scope().get_core().guess_name(*this));
#endif

        if (!items.empty()) // we add the fields of the item..
            j_itm["exprs"] = env::to_json();

        return j_itm;
    }

    std::shared_ptr<bool_item> component::operator==(std::shared_ptr<item> rhs) const { return get_core().new_bool(this == rhs.get()); }

    atom::atom(predicate &t, bool fact, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args) : item(t), env(t.get_core(), atom_parent(t, args), std::move(args)), fact(fact) {}
    json::json atom::to_json() const
    {
        json::json j_atm{{"is_fact", fact}, {"type", get_type().get_full_name()}};

        // we add the state of the atom..
        switch (get_state())
        {
        case atom_state::active:
            j_atm["state"] = "active";
            break;
        case atom_state::inactive:
            j_atm["state"] = "inactive";
            break;
        case atom_state::unified:
            j_atm["state"] = "unified";
            break;
        }

        if (!items.empty()) // we add the fields of the atom..
            j_atm["exprs"] = env::to_json();

        return j_atm;
    }

    env &atom::atom_parent(const predicate &t, const std::map<std::string, std::shared_ptr<item>, std::less<>> &args)
    {
        if (args.count(tau_kw))
            return *static_cast<component *>(args.at(tau_kw).get());
        else
            return t.get_core();
    }
} // namespace riddle