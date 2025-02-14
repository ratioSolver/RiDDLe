#pragma once

#include "term.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class bool_item : public bool_itm
  {
  public:
    bool_item(bool_type &tp, utils::lit &&expr) : bool_itm(tp), expr(expr) {}

    [[nodiscard]] const utils::lit &get_lit() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = bool_itm::to_json();
      j_val["lit"] = to_string(expr).c_str();
      return j_val;
    }

  private:
    const utils::lit expr;
  };

  class arith_item : public arith_itm
  {
  public:
    arith_item(int_type &tp, utils::lin &&expr) : arith_itm(tp), expr(expr) {}
    arith_item(real_type &tp, utils::lin &&expr) : arith_itm(tp), expr(expr) {}
    arith_item(time_type &tp, utils::lin &&expr) : arith_itm(tp), expr(expr) {}

    [[nodiscard]] const utils::lin &get_lin() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = arith_itm::to_json();
      j_val["lin"] = to_string(expr).c_str();
      return j_val;
    }

  private:
    const utils::lin expr;
  };

  class string_item : public string_itm
  {
  public:
    string_item(string_type &tp, std::string &&expr) : string_itm(tp), expr(expr) {}

    [[nodiscard]] const std::string &get_string() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = string_itm::to_json();
      j_val["str"] = expr;
      return j_val;
    }

  private:
    const std::string expr;
  };

  class enum_item : public enum_itm
  {
  public:
    enum_item(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values, utils::var &&expr) : enum_itm(tp, std::move(values)), expr(expr) {}

    [[nodiscard]] const utils::var &get_var() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_val = enum_itm::to_json();
      j_val["var"] = expr;
      return j_val;
    }

  private:
    const utils::var expr;
  };

  class atom : public riddle::atm
  {
  public:
    atom(riddle::predicate &pred, bool is_fact, std::map<std::string, utils::s_ptr<riddle::item>, std::less<>> &&args, utils::lit &&sigma) : riddle::atm(pred, is_fact, std::move(args)), sigma(sigma) {}

    [[nodiscard]] const utils::lit &get_sigma() const noexcept { return sigma; }

    [[nodiscard]] virtual json::json to_json() const override
    {
      auto j_atm = riddle::atm::to_json();
      j_atm["sigma"] = to_string(sigma).c_str();
      return j_atm;
    }

  private:
    const utils::lit sigma;
  };
} // namespace riddle
