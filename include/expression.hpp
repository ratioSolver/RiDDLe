#pragma once

#include <vector>
#include "item.hpp"
#include "lexer.hpp"

namespace riddle
{
  class scope;

  class expression
  {
  public:
    expression() = default;
    virtual ~expression() = default;

    virtual std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const = 0;

    virtual std::string to_string() const = 0;
  };

  class bool_expression final : public expression
  {
  public:
    bool_expression(const bool_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    bool_token l;
  };

  class int_expression final : public expression
  {
  public:
    int_expression(const int_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    int_token l;
  };

  class real_expression final : public expression
  {
  public:
    real_expression(const real_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    real_token l;
  };

  class string_expression final : public expression
  {
  public:
    string_expression(const string_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    string_token l;
  };

  class cast_expression final : public expression
  {
  public:
    cast_expression(std::vector<id_token> &&tp, std::unique_ptr<expression> &&xpr) : cast_to_type(std::move(tp)), expr(std::move(xpr)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = "(" + cast_to_type[0].to_string();
      for (size_t i = 1; i < cast_to_type.size(); ++i)
        res += "." + cast_to_type[i].to_string();
      return res + ") " + expr->to_string();
    }

  private:
    std::vector<id_token> cast_to_type;
    std::unique_ptr<expression> expr;
  };

  class plus_expression final : public expression
  {
  public:
    plus_expression(std::unique_ptr<expression> &&xpr) : expr(std::move(xpr)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return "+" + expr->to_string(); }

  private:
    std::unique_ptr<expression> expr;
  };

  class minus_expression final : public expression
  {
  public:
    minus_expression(std::unique_ptr<expression> &&xpr) : expr(std::move(xpr)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return "-" + expr->to_string(); }

  private:
    std::unique_ptr<expression> expr;
  };

  class not_expression final : public expression
  {
  public:
    not_expression(std::unique_ptr<expression> &&xpr) : expr(std::move(xpr)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return "!" + expr->to_string(); }

  private:
    std::unique_ptr<expression> expr;
  };

  class constructor_expression final : public expression
  {
  public:
    constructor_expression(std::vector<id_token> &&tp, std::vector<std::unique_ptr<expression>> &&args) : instance_type(std::move(tp)), args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = "(" + instance_type[0].to_string();
      for (size_t i = 1; i < instance_type.size(); ++i)
        res += "." + instance_type[i].to_string();
      res += ")(";
      for (size_t i = 0; i < args.size(); ++i)
      {
        if (i > 0)
          res += ", ";
        res += args[i]->to_string();
      }
      return res + ")";
    }

  private:
    std::vector<id_token> instance_type;
    std::vector<std::unique_ptr<expression>> args;
  };

  class eq_expression final : public expression
  {
  public:
    eq_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " == " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class neq_expression final : public expression
  {
  public:
    neq_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " != " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class lt_expression final : public expression
  {
  public:
    lt_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " < " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class leq_expression final : public expression
  {
  public:
    leq_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " <= " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class geq_expression final : public expression
  {
  public:
    geq_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " >= " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class gt_expression final : public expression
  {
  public:
    gt_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " > " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class function_expression final : public expression
  {
  public:
    function_expression(std::vector<id_token> &&obj_id, id_token &&fn_name, std::vector<std::unique_ptr<expression>> &&args) : object_id(std::move(obj_id)), function_name(std::move(fn_name)), args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = "(" + object_id[0].to_string();
      for (size_t i = 1; i < object_id.size(); ++i)
        res += "." + object_id[i].to_string();
      res += ")." + function_name.to_string() + "(";
      for (size_t i = 0; i < args.size(); ++i)
      {
        if (i > 0)
          res += ", ";
        res += args[i]->to_string();
      }
      return res + ")";
    }

  private:
    std::vector<id_token> object_id;
    id_token function_name;
    std::vector<std::unique_ptr<expression>> args;
  };

  class id_expression final : public expression
  {
  public:
    id_expression(std::vector<id_token> &&obj_id) : object_id(std::move(obj_id)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = object_id[0].to_string();
      for (size_t i = 1; i < object_id.size(); ++i)
        res += "." + object_id[i].to_string();
      return res;
    }

  private:
    std::vector<id_token> object_id;
  };

  class implication_expression final : public expression
  {
  public:
    implication_expression(std::unique_ptr<expression> &&l, std::unique_ptr<expression> &&r) : l(std::move(l)), r(std::move(r)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override { return l->to_string() + " -> " + r->to_string(); }

  private:
    std::unique_ptr<expression> l;
    std::unique_ptr<expression> r;
  };

  class disjunction_expression final : public expression
  {
  public:
    disjunction_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " | " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class conjunction_expression final : public expression
  {
  public:
    conjunction_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " & " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class xor_expression final : public expression
  {
  public:
    xor_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " ^ " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class addition_expression final : public expression
  {
  public:
    addition_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " + " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class subtraction_expression final : public expression
  {
  public:
    subtraction_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " - " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class multiplication_expression final : public expression
  {
  public:
    multiplication_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " * " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };

  class division_expression final : public expression
  {
  public:
    division_expression(std::vector<std::unique_ptr<expression>> &&args) : args(std::move(args)) {}

    std::shared_ptr<item> evaluate(scope &scp, std::shared_ptr<env> &ctx) const override;

    std::string to_string() const override
    {
      std::string res = args[0]->to_string();
      for (size_t i = 1; i < args.size(); ++i)
        res += " / " + args[i]->to_string();
      return res;
    }

  private:
    std::vector<std::unique_ptr<expression>> args;
  };
} // namespace riddle
