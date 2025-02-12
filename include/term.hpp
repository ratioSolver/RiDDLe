#pragma once

#include "memory.hpp"
#include "bool.hpp"
#include "integer.hpp"
#include "rational.hpp"
#include <vector>

namespace riddle
{
  class core;

  class term
  {
  public:
    term(core &cr) : cr(cr) {}
    virtual ~term() = default;

    [[nodiscard]] core &get_core() const noexcept { return cr; }

  private:
    core &cr;
  };

  using expr = utils::s_ptr<term>;

  class bool_term : public term
  {
  public:
    bool_term(core &cr) : term(cr) {}

    [[nodiscard]] virtual utils::lbool value() const noexcept = 0;
  };

  using bool_expr = utils::s_ptr<bool_term>;

  class bool_const final : public bool_term
  {
  public:
    bool_const(core &cr, utils::lbool val) : bool_term(cr), val(val) {}

    [[nodiscard]] utils::lbool value() const noexcept override { return val; }

  private:
    const utils::lbool val;
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

    [[nodiscard]] utils::lbool value() const noexcept override { return !arg->value(); }

  private:
    const bool_expr arg;
  };

  class bool_and final : public bool_term
  {
  public:
    bool_and(core &cr, std::vector<bool_expr> args) : bool_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      for (const auto &arg : args)
        if (arg->value() == utils::False)
          return utils::False;
      return utils::True;
    }

  private:
    const std::vector<bool_expr> args;
  };

  class bool_or final : public bool_term
  {
  public:
    bool_or(core &cr, std::vector<bool_expr> args) : bool_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      for (const auto &arg : args)
        if (arg->value() == utils::True)
          return utils::True;
      return utils::False;
    }

  private:
    const std::vector<bool_expr> args;
  };

  class int_term : public term
  {
  public:
    int_term(core &cr) : term(cr) {}

    [[nodiscard]] virtual utils::integer value() const noexcept = 0;

    [[nodiscard]] virtual utils::integer lb() const noexcept = 0;
    [[nodiscard]] virtual utils::integer ub() const noexcept = 0;
  };

  using int_expr = utils::s_ptr<int_term>;

  class int_const final : public int_term
  {
  public:
    int_const(core &cr, utils::integer val) : int_term(cr), val(val) {}

    [[nodiscard]] utils::integer value() const noexcept override { return val; }
    [[nodiscard]] utils::integer lb() const noexcept override { return val; }
    [[nodiscard]] utils::integer ub() const noexcept override { return val; }

  private:
    const utils::integer val;
  };

  class int_var : public int_term
  {
  public:
    int_var(core &cr) : int_term(cr) {}
  };

  class int_add final : public int_term
  {
  public:
    int_add(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::integer value() const noexcept override
    {
      utils::integer sum = utils::integer::zero;
      for (const auto &arg : args)
        sum += arg->value();
      return sum;
    }

    [[nodiscard]] utils::integer lb() const noexcept override
    {
      utils::integer sum = utils::integer::zero;
      for (const auto &arg : args)
        sum += arg->lb();
      return sum;
    }

    [[nodiscard]] utils::integer ub() const noexcept override
    {
      utils::integer sum = utils::integer::zero;
      for (const auto &arg : args)
        sum += arg->ub();
      return sum;
    }

  private:
    const std::vector<int_expr> args;
  };

  class int_sub final : public int_term
  {
  public:
    int_sub(core &cr, int_expr lhs, int_expr rhs) : int_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::integer value() const noexcept override { return lhs->value() - rhs->value(); }
    [[nodiscard]] utils::integer lb() const noexcept override { return lhs->lb() - rhs->ub(); }
    [[nodiscard]] utils::integer ub() const noexcept override { return lhs->ub() - rhs->lb(); }

  private:
    const int_expr lhs, rhs;
  };

  class int_mul final : public int_term
  {
  public:
    int_mul(core &cr, std::vector<int_expr> args) : int_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::integer value() const noexcept override
    {
      utils::integer prod = utils::integer::one;
      for (const auto &arg : args)
        prod *= arg->value();
      return prod;
    }

    [[nodiscard]] utils::integer lb() const noexcept override
    {
      utils::integer prod = utils::integer::one;
      for (const auto &arg : args)
        prod *= arg->lb();
      return prod;
    }

    [[nodiscard]] utils::integer ub() const noexcept override
    {
      utils::integer prod = utils::integer::one;
      for (const auto &arg : args)
        prod *= arg->ub();
      return prod;
    }

  private:
    const std::vector<int_expr> args;
  };

  class int_div final : public int_term
  {
  public:
    int_div(core &cr, int_expr lhs, int_expr rhs) : int_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::integer value() const noexcept override { return lhs->value() / rhs->value(); }
    [[nodiscard]] utils::integer lb() const noexcept override { return lhs->lb() / rhs->ub(); }
    [[nodiscard]] utils::integer ub() const noexcept override { return lhs->ub() / rhs->lb(); }

  private:
    const int_expr lhs, rhs;
  };

  class int_lt final : public bool_term
  {
  public:
    int_lt(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() < rhs->lb())
        return utils::True;
      if (lhs->lb() >= rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const int_expr lhs, rhs;
  };

  class int_le final : public bool_term
  {
  public:
    int_le(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() <= rhs->lb())
        return utils::True;
      if (lhs->lb() > rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const int_expr lhs, rhs;
  };

  class int_eq final : public bool_term
  {
  public:
    int_eq(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() < rhs->lb() || lhs->lb() > rhs->ub())
        return utils::False;
      if (lhs->lb() == rhs->lb() && lhs->ub() == rhs->ub())
        return utils::True;
      return utils::Undefined;
    }

  private:
    const int_expr lhs, rhs;
  };

  class int_ge final : public bool_term
  {
  public:
    int_ge(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() >= rhs->lb())
        return utils::True;
      if (lhs->lb() < rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const int_expr lhs, rhs;
  };

  class int_gt final : public bool_term
  {
  public:
    int_gt(core &cr, int_expr lhs, int_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() > rhs->lb())
        return utils::True;
      if (lhs->lb() <= rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const int_expr lhs, rhs;
  };

  class real_term : public term
  {
  public:
    real_term(core &cr) : term(cr) {}

    [[nodiscard]] virtual utils::rational value() const noexcept = 0;

    [[nodiscard]] virtual utils::rational lb() const noexcept = 0;
    [[nodiscard]] virtual utils::rational ub() const noexcept = 0;
  };

  using real_expr = utils::s_ptr<real_term>;

  class real_const final : public real_term
  {
  public:
    real_const(core &cr, utils::rational val) : real_term(cr), val(val) {}

    [[nodiscard]] utils::rational value() const noexcept override { return val; }
    [[nodiscard]] utils::rational lb() const noexcept override { return val; }
    [[nodiscard]] utils::rational ub() const noexcept override { return val; }

  private:
    const utils::rational val;
  };

  class real_var : public real_term
  {
  public:
    real_var(core &cr) : real_term(cr) {}
  };

  class real_add final : public real_term
  {
  public:
    real_add(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::rational value() const noexcept override
    {
      utils::rational sum = utils::rational::zero;
      for (const auto &arg : args)
        sum += arg->value();
      return sum;
    }

    [[nodiscard]] utils::rational lb() const noexcept override
    {
      utils::rational sum = utils::rational::zero;
      for (const auto &arg : args)
        sum += arg->lb();
      return sum;
    }

    [[nodiscard]] utils::rational ub() const noexcept override
    {
      utils::rational sum = utils::rational::zero;
      for (const auto &arg : args)
        sum += arg->ub();
      return sum;
    }

  private:
    const std::vector<real_expr> args;
  };

  class real_sub final : public real_term
  {
  public:
    real_sub(core &cr, real_expr lhs, real_expr rhs) : real_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::rational value() const noexcept override { return lhs->value() - rhs->value(); }
    [[nodiscard]] utils::rational lb() const noexcept override { return lhs->lb() - rhs->ub(); }
    [[nodiscard]] utils::rational ub() const noexcept override { return lhs->ub() - rhs->lb(); }

  private:
    const real_expr lhs, rhs;
  };

  class real_mul final : public real_term
  {
  public:
    real_mul(core &cr, std::vector<real_expr> args) : real_term(cr), args(std::move(args)) {}

    [[nodiscard]] utils::rational value() const noexcept override
    {
      utils::rational prod = utils::rational::one;
      for (const auto &arg : args)
        prod *= arg->value();
      return prod;
    }

    [[nodiscard]] utils::rational lb() const noexcept override
    {
      utils::rational prod = utils::rational::one;
      for (const auto &arg : args)
        prod *= arg->lb();
      return prod;
    }

    [[nodiscard]] utils::rational ub() const noexcept override
    {
      utils::rational prod = utils::rational::one;
      for (const auto &arg : args)
        prod *= arg->ub();
      return prod;
    }

  private:
    const std::vector<real_expr> args;
  };

  class real_div final : public real_term
  {
  public:
    real_div(core &cr, real_expr lhs, real_expr rhs) : real_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::rational value() const noexcept override { return lhs->value() / rhs->value(); }
    [[nodiscard]] utils::rational lb() const noexcept override { return lhs->lb() / rhs->ub(); }
    [[nodiscard]] utils::rational ub() const noexcept override { return lhs->ub() / rhs->lb(); }

  private:
    const real_expr lhs, rhs;
  };

  class real_lt final : public bool_term
  {
  public:
    real_lt(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() < rhs->lb())
        return utils::True;
      if (lhs->lb() >= rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const real_expr lhs, rhs;
  };

  class real_le final : public bool_term
  {
  public:
    real_le(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() <= rhs->lb())
        return utils::True;
      if (lhs->lb() > rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const real_expr lhs, rhs;
  };

  class real_eq final : public bool_term
  {
  public:
    real_eq(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() < rhs->lb() || lhs->lb() > rhs->ub())
        return utils::False;
      if (lhs->lb() == rhs->lb() && lhs->ub() == rhs->ub())
        return utils::True;
      return utils::Undefined;
    }

  private:
    const real_expr lhs, rhs;
  };

  class real_ge final : public bool_term
  {
  public:
    real_ge(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() >= rhs->lb())
        return utils::True;
      if (lhs->lb() < rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
    const real_expr lhs, rhs;
  };

  class real_gt final : public bool_term
  {
  public:
    real_gt(core &cr, real_expr lhs, real_expr rhs) : bool_term(cr), lhs(lhs), rhs(rhs) {}

    [[nodiscard]] utils::lbool value() const noexcept override
    {
      if (lhs->ub() > rhs->lb())
        return utils::True;
      if (lhs->lb() <= rhs->ub())
        return utils::False;
      return utils::Undefined;
    }

  private:
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

    [[nodiscard]] std::string value() const noexcept override { return val; }

  private:
    const std::string val;
  };

  [[nodiscard]] bool_expr operator&&(bool_expr lhs, bool_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator&&(bool_expr lhs, bool rhs) noexcept;
  [[nodiscard]] bool_expr operator&&(bool lhs, bool_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator||(bool_expr lhs, bool_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator||(bool_expr lhs, bool rhs) noexcept;
  [[nodiscard]] bool_expr operator||(bool lhs, bool_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator!(bool_expr arg) noexcept;

  [[nodiscard]] int_expr operator+(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator+(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] int_expr operator+(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator+(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] int_expr operator+(int lhs, int_expr rhs) noexcept;

  [[nodiscard]] int_expr operator-(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator-(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] int_expr operator-(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator-(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] int_expr operator-(int lhs, int_expr rhs) noexcept;

  [[nodiscard]] int_expr operator*(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator*(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] int_expr operator*(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator*(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] int_expr operator*(int lhs, int_expr rhs) noexcept;

  [[nodiscard]] int_expr operator/(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator/(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] int_expr operator/(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] int_expr operator/(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] int_expr operator/(int lhs, int_expr rhs) noexcept;

  [[nodiscard]] bool_expr operator<(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] bool_expr operator<(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] bool_expr operator<(int lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(int lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] bool_expr operator==(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] bool_expr operator==(int lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(int lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(int_expr lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(int_expr lhs, utils::integer rhs) noexcept;
  [[nodiscard]] bool_expr operator>(utils::integer lhs, int_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(int_expr lhs, int rhs) noexcept;
  [[nodiscard]] bool_expr operator>(int lhs, int_expr rhs) noexcept;

  [[nodiscard]] real_expr operator+(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator+(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] real_expr operator+(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator+(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] real_expr operator+(double lhs, real_expr rhs) noexcept;

  [[nodiscard]] real_expr operator-(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator-(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] real_expr operator-(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator-(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] real_expr operator-(double lhs, real_expr rhs) noexcept;

  [[nodiscard]] real_expr operator*(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator*(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] real_expr operator*(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator*(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] real_expr operator*(double lhs, real_expr rhs) noexcept;

  [[nodiscard]] real_expr operator/(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator/(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] real_expr operator/(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] real_expr operator/(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] real_expr operator/(double lhs, real_expr rhs) noexcept;

  [[nodiscard]] bool_expr operator<(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] bool_expr operator<(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] bool_expr operator<(double lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] bool_expr operator<=(double lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] bool_expr operator==(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator==(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] bool_expr operator==(double lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] bool_expr operator>=(double lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(real_expr lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(real_expr lhs, utils::rational rhs) noexcept;
  [[nodiscard]] bool_expr operator>(utils::rational lhs, real_expr rhs) noexcept;
  [[nodiscard]] bool_expr operator>(real_expr lhs, double rhs) noexcept;
  [[nodiscard]] bool_expr operator>(double lhs, real_expr rhs) noexcept;
} // namespace riddle
