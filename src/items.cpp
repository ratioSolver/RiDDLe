#include "items.hpp"
#include "core.hpp"

namespace riddle
{
    bool_item::bool_item(bool_type &tp, utils::lit &&expr) noexcept : bool_term(tp), expr(expr) {}

    json::json bool_item::to_json() const
    {
        auto j_val = bool_term::to_json();
        j_val["lit"] = to_string(expr).c_str();
        return j_val;
    }

    arith_item::arith_item(int_type &tp, utils::lin &&expr) noexcept : arith_term(tp), expr(expr) {}
    arith_item::arith_item(real_type &tp, utils::lin &&expr) noexcept : arith_term(tp), expr(expr) {}
    arith_item::arith_item(time_type &tp, utils::lin &&expr) noexcept : arith_term(tp), expr(expr) {}

    json::json arith_item::to_json() const
    {
        auto j_val = arith_term::to_json();
        j_val["lin"] = to_string(expr).c_str();
        return j_val;
    }

    string_item::string_item(string_type &tp, std::string &&expr) noexcept : string_term(tp), expr(expr) {}

    enum_item::enum_item(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values, std::vector<utils::lit> &&lits) noexcept : enum_term(tp, std::move(values))
    {
        for (size_t i = 0; i < get_values().size(); i++)
            domain.emplace(&*get_values()[i], lits[i]);
    }

    json::json enum_item::to_json() const
    {
        auto j_val = enum_term::to_json();
        json::json j_vals(json::json_type::array);
        for (const auto &val : get_type().get_scope().get_core().enum_value(*this))
            j_vals.push_back(static_cast<uint64_t>(static_cast<term &>(*val).get_id()));
        j_val["vals"] = std::move(j_vals);
        return j_val;
    }

    atom::atom(riddle::predicate &pred, bool is_fact, std::map<std::string, utils::s_ptr<riddle::term>, std::less<>> &&args, utils::lit &&sigma) noexcept : riddle::atom_term(pred, is_fact, std::move(args)), sigma(sigma) {}

    json::json atom::to_json() const
    {
        auto j_atm = riddle::atom_term::to_json();
        j_atm["sigma"] = to_string(sigma).c_str();
        return j_atm;
    }
} // namespace riddle
