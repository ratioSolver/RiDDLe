#pragma once

#include "lexer.hpp"
#include <map>

namespace riddle
{
  class core;

  class term
  {
  public:
    term(core &cr) : cr(cr) {}
    virtual ~term() = default;

    core &cr;
  };

  using expr = utils::s_ptr<term>;

  class bool_term : public term
  {
  public:
    bool_term(core &cr) : term(cr) {}
  };

  using bool_expr = utils::s_ptr<bool_term>;

  class bool_const final : public bool_term
  {
  public:
    bool_const(core &cr, bool_token &&val) : bool_term(cr), val(val) {}

    const bool_token val;
  };

  class bool_var : public bool_term
  {
  public:
    bool_var(core &cr) : bool_term(cr) {}
  };

  class bool_not final : public bool_term
  {
  public:
    bool_not(core &cr, bool_expr arg) : bool_term(cr), arg(arg) {}

    const bool_expr arg;
  };

  class bool_and final : public bool_term
  {
  public:
    bool_and(core &cr, std::vector<bool_expr> args) : bool_term(cr), args(std::move(args)) {}

    const std::vector<bool_expr> args;
  };

  class bool_or final : public bool_term
  {
  public:
    bool_or(core &cr, std::vector<bool_expr> args) : bool_term(cr), args(std::move(args)) {}

    const std::vector<bool_expr> args;
  };

  class bool_xor final : public bool_term
  {
  public:
    bool_xor(core &cr, std::vector<bool_expr> args) : bool_term(cr), args(std::move(args)) {}

    const std::vector<bool_expr> args;
  };

  class int_term : public term
  {
  public:
    int_term(core &cr) : term(cr) {}
  };

  using int_expr = utils::s_ptr<int_term>;

  class int_const final : public int_term
  {
  public:
    int_const(core &cr, int_token &&val) : int_term(cr), val(val) {}

    const int_token val;
  };

  class int_var : public int_term
  {
  public:
    int_var(core &cr) : int_term(cr) {}
  };

  class bounded_int_var : public int_term
  {
  public:
    bounded_int_var(core &cr, int_token &&lb, int_token &&ub) : int_term(cr), lb(lb), ub(ub) {}

    const int_token lb, ub;
  };

  class uncertain_int_var : public int_term
  {
  public:
    uncertain_int_var(core &cr, int_token &&lb, int_token &&ub) : int_term(cr), lb(lb), ub(ub) {}

    const int_token lb, ub;
  };

  class int_add final : public int_term
  {
  public:
    int_add(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    const std::vector<int_expr> args;
  };

  class int_sub final : public int_term
  {
  public:
    int_sub(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    const std::vector<int_expr> args;
  };

  class int_mul final : public int_term
  {
  public:
    int_mul(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    const std::vector<int_expr> args;
  };

  class int_div final : public int_term
  {
  public:
    int_div(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    const std::vector<int_expr> args;
  };

  class int_lt final : public bool_term
  {
  public:
    int_lt(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const int_expr lhs, rhs;
  };

  class int_le final : public bool_term
  {
  public:
    int_le(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const int_expr lhs, rhs;
  };

  class int_eq final : public bool_term
  {
  public:
    int_eq(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const int_expr lhs, rhs;
  };

  class int_ge final : public bool_term
  {
  public:
    int_ge(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const int_expr lhs, rhs;
  };

  class int_gt final : public bool_term
  {
  public:
    int_gt(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const int_expr lhs, rhs;
  };

  class real_term : public term
  {
  public:
    real_term(core &cr) : term(cr) {}
  };

  using real_expr = utils::s_ptr<real_term>;

  class real_const final : public real_term
  {
  public:
    real_const(core &cr, real_token &&val) : real_term(cr), val(val) {}

    const real_token val;
  };

  class real_var : public real_term
  {
  public:
    real_var(core &cr) : real_term(cr) {}
  };

  class bounded_real_var : public real_term
  {
  public:
    bounded_real_var(core &cr, real_token &&lb, real_token &&ub) : real_term(cr), lb(lb), ub(ub) {}

    const real_token lb, ub;
  };

  class uncertain_real_var : public real_term
  {
  public:
    uncertain_real_var(core &cr, real_token &&lb, real_token &&ub) : real_term(cr), lb(lb), ub(ub) {}

    const real_token lb, ub;
  };

  class real_add final : public real_term
  {
  public:
    real_add(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    const std::vector<real_expr> args;
  };

  class real_sub final : public real_term
  {
  public:
    real_sub(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    const std::vector<real_expr> args;
  };

  class real_mul final : public real_term
  {
  public:
    real_mul(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    const std::vector<real_expr> args;
  };

  class real_div final : public real_term
  {
  public:
    real_div(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    const std::vector<real_expr> args;
  };

  class real_lt final : public bool_term
  {
  public:
    real_lt(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const real_expr lhs, rhs;
  };

  class real_le final : public bool_term
  {
  public:
    real_le(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const real_expr lhs, rhs;
  };

  class real_eq final : public bool_term
  {
  public:
    real_eq(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const real_expr lhs, rhs;
  };

  class real_ge final : public bool_term
  {
  public:
    real_ge(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const real_expr lhs, rhs;
  };

  class real_gt final : public bool_term
  {
  public:
    real_gt(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    const real_expr lhs, rhs;
  };

  class string_term : public term
  {
  public:
    string_term(core &cr) : term(cr) {}

    [[nodiscard]] virtual std::string value() const noexcept = 0;
  };

  using string_expr = utils::s_ptr<string_term>;

  class string_const final : public string_term
  {
  public:
    string_const(core &cr, std::string val) : string_term(cr), val(std::move(val)) {}

    const std::string val;
  };

  bool_expr push_negations(bool_expr expr) noexcept;
  bool_expr distribute(bool_expr expr) noexcept;
  bool_expr to_cnf(bool_expr expr) noexcept { return distribute(push_negations(expr)); }

  std::map<int_expr, INT_TYPE> linearize(int_expr expr);
  std::map<real_expr, utils::rational> linearize(real_expr expr);
} // namespace riddle
