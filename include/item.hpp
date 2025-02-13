#pragma once

#include "itm.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class bool_item : public bool_itm
  {
  public:
    bool_item(bool_type &tp, utils::lit &&expr) : bool_itm(tp), expr(expr) {}

    [[nodiscard]] const utils::lit &get_lit() const noexcept { return expr; }

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

  private:
    const utils::lin expr;
  };

  class string_item : public string_itm
  {
  public:
    string_item(string_type &tp, std::string &&expr) : string_itm(tp), expr(expr) {}

    [[nodiscard]] const std::string &get_expr() const noexcept { return expr; }

  private:
    const std::string expr;
  };

  class enum_item : public enum_itm
  {
  public:
    enum_item(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values, utils::var &&expr) : enum_itm(tp, std::move(values)), expr(expr) {}

    [[nodiscard]] const utils::var &get_var() const noexcept { return expr; }

  private:
    const utils::var expr;
  };
} // namespace riddle
