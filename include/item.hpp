#pragma once

#include "term.hpp"
#include "lit.hpp"
#include "lin.hpp"
#include <unordered_map>

namespace riddle
{
  class bool_item : public bool_term
  {
  public:
    bool_item(bool_type &tp, utils::lit &&expr) : bool_term(tp), expr(expr) {}

    [[nodiscard]] const utils::lit &get_lit() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = bool_term::to_json();
      j_val["lit"] = to_string(expr).c_str();
      return j_val;
    }

  private:
    const utils::lit expr;
  };

  class arith_item : public arith_term
  {
  public:
    arith_item(int_type &tp, utils::lin &&expr) : arith_term(tp), expr(expr) {}
    arith_item(real_type &tp, utils::lin &&expr) : arith_term(tp), expr(expr) {}
    arith_item(time_type &tp, utils::lin &&expr) : arith_term(tp), expr(expr) {}

    [[nodiscard]] const utils::lin &get_lin() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = arith_term::to_json();
      j_val["lin"] = to_string(expr).c_str();
      return j_val;
    }

  private:
    const utils::lin expr;
  };

  class string_item : public string_term
  {
  public:
    string_item(string_type &tp, std::string &&expr) : string_term(tp), expr(expr) {}

    [[nodiscard]] const std::string &get_string() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = string_term::to_json();
      j_val["str"] = expr;
      return j_val;
    }

  private:
    const std::string expr;
  };

  class enum_item : public enum_term
  {
  public:
    enum_item(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values, std::vector<utils::lit> &&lits) : enum_term(tp, std::move(values))
    {
      for (size_t i = 0; i < get_values().size(); i++)
        domain.emplace(&*get_values()[i], lits[i]);
    }

    [[nodiscard]] bool has_lit(const utils::enum_val &val) const noexcept { return domain.find(&val) != domain.end(); }

    [[nodiscard]] const utils::lit &get_lit(const utils::enum_val &val) const noexcept { return domain.at(&val); }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = enum_term::to_json();
      json::json j_vals(json::json_type::array);
      for (const auto &val : get_type().get_scope().get_core().enum_value(*this))
        j_vals.push_back(static_cast<uint64_t>(static_cast<term &>(*val).get_id()));
      j_val["vals"] = std::move(j_vals);
      return j_val;
    }

  private:
    std::unordered_map<const utils::enum_val *, const utils::lit> domain;
  };

  class atom : public riddle::atom_term
  {
  public:
    atom(riddle::predicate &pred, bool is_fact, std::map<std::string, utils::s_ptr<riddle::term>, std::less<>> &&args, utils::lit &&sigma) : riddle::atom_term(pred, is_fact, std::move(args)), sigma(sigma) {}

    [[nodiscard]] const utils::lit &get_sigma() const noexcept { return sigma; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_atm = riddle::atom_term::to_json();
      j_atm["sigma"] = to_string(sigma).c_str();
      return j_atm;
    }

  private:
    const utils::lit sigma;
  };
} // namespace riddle
