#include "core.hpp"
#include <cassert>

namespace riddle
{
    bool_term::bool_term(bool_type &tp) noexcept : term(tp) {}
    json::json bool_term::to_json() const noexcept
    {
        json::json j_val{{"type", get_type().get_name()}}; // we add the type of the item..
        switch (get_type().get_scope().get_core().bool_value(*this))
        {
        case utils::True:
            j_val["val"] = true;
            break;
        case utils::False:
            j_val["val"] = false;
            break;
        }
        return j_val;
    }

    arith_term::arith_term(int_type &tp) noexcept : term(tp) {}
    arith_term::arith_term(real_type &tp) noexcept : term(tp) {}
    arith_term::arith_term(time_type &tp) noexcept : term(tp) {}
    json::json arith_term::to_json() const noexcept
    {
        json::json j_val{{"type", get_type().get_name()}}; // we add the type of the item..
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

    string_term::string_term(string_type &tp) noexcept : term(tp) {}
    json::json string_term::to_json() const noexcept { return {{"type", get_type().get_name()}, {"val", get_type().get_scope().get_core().string_value(*this)}}; }

    enum_term::enum_term(component_type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) noexcept : term(tp), env(tp.get_core(), tp.get_core()), values(std::move(values)) {}
    json::json enum_term::to_json() const noexcept
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
            j_vals.push_back(static_cast<uint64_t>(static_cast<term &>(val.get()).get_id()));
        j_val["vals"] = std::move(j_vals);

        return j_val;
    }

    component::component(component_type &t) noexcept : term(t), env(t.get_core(), t.get_core()) {}
    json::json component::to_json() const noexcept
    {
        json::json j_itm{{"type", get_type().get_full_name()}}; // we add the type of the item..
#ifdef COMPUTE_NAMES
        j_itm["name"] = get_type().get_scope().get_core().guess_name(*this);
#endif

        if (!items.empty()) // we add the fields of the item..
            j_itm["exprs"] = env::to_json();

        return j_itm;
    }

    atom_term::atom_term(predicate &t, bool fact, std::map<std::string, expr, std::less<>> &&args) noexcept : term(t), env(t.get_core(), atom_parent(t, args), std::move(args)), fact(fact) {}
    atom_state atom_term::get_state() const noexcept { return get_type().get_scope().get_core().get_atom_state(*this); }
    json::json atom_term::to_json() const noexcept
    {
        json::json j_atm{{"is_fact", fact}, {"type", get_type().get_full_name()}};

        // we add the state of the atom..
        switch (get_type().get_scope().get_core().get_atom_state(*this))
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

    env &atom_term::atom_parent(const predicate &t, const std::map<std::string, expr, std::less<>> &args)
    {
        if (args.count(tau_kw))
        {
            auto tau = args.at(tau_kw);
            if (auto c_tau = dynamic_cast<component *>(tau.get()))
                return *c_tau;
            else if (auto c_tau = dynamic_cast<enum_term *>(tau.get()))
                return *c_tau;
            else
                throw std::runtime_error("Invalid object reference");
        }
        else
            return t.get_core();
    }

    bool_expr push_negations(bool_expr expr) noexcept
    {
        if (auto not_xpr = std::dynamic_pointer_cast<bool_not>(expr))
        {
            if (auto not_not_xpr = std::dynamic_pointer_cast<bool_not>(not_xpr->arg))
                return push_negations(not_not_xpr->arg);
            else if (auto and_xpr = std::dynamic_pointer_cast<and_term>(not_xpr->arg))
            {
                std::vector<bool_expr> args;
                for (const auto &arg : and_xpr->args)
                    args.push_back(push_negations(expr->get_type().get_scope().get_core().new_not(arg)));
                return expr->get_type().get_scope().get_core().new_or(std::move(args));
            }
            else if (auto or_xpr = std::dynamic_pointer_cast<or_term>(not_xpr->arg))
            {
                std::vector<bool_expr> args;
                for (const auto &arg : or_xpr->args)
                    args.push_back(push_negations(expr->get_type().get_scope().get_core().new_not(arg)));
                return expr->get_type().get_scope().get_core().new_and(std::move(args));
            }
            else
                return not_xpr;
        }
        else if (auto and_xpr = std::dynamic_pointer_cast<and_term>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : and_xpr->args)
                args.push_back(push_negations(arg));
            return expr->get_type().get_scope().get_core().new_and(std::move(args));
        }
        else if (auto or_xpr = std::dynamic_pointer_cast<or_term>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : or_xpr->args)
                args.push_back(push_negations(arg));
            return expr->get_type().get_scope().get_core().new_or(std::move(args));
        }
        else
            return expr;
    }

    bool_expr distribute(bool_expr expr) noexcept
    {
        if (auto or_xpr = std::dynamic_pointer_cast<or_term>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : or_xpr->args)
                args.push_back(distribute(arg));
            std::vector<bool_expr> new_args;
            for (const auto &arg : args)
                if (auto and_xpr = std::dynamic_pointer_cast<and_term>(arg))
                    for (const auto &a : and_xpr->args)
                        new_args.push_back(a);
                else
                    new_args.push_back(arg);
            return expr->get_type().get_scope().get_core().new_or(std::move(new_args));
        }
        else if (auto and_xpr = std::dynamic_pointer_cast<and_term>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : and_xpr->args)
                args.push_back(distribute(arg));
            return expr->get_type().get_scope().get_core().new_and(std::move(args));
        }
        else
            return expr;
    }

    bool is_bool(expr xpr) noexcept { return is_bool(xpr->get_type()); }
    bool is_int(expr xpr) noexcept { return is_int(xpr->get_type()); }
    bool is_real(expr xpr) noexcept { return is_real(xpr->get_type()); }
    bool is_time(expr xpr) noexcept { return is_time(xpr->get_type()); }
} // namespace riddle