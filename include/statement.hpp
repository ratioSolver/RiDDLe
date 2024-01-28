#pragma once

#include "expression.hpp"

namespace riddle
{
  class statement
  {
  public:
    statement() = default;
    virtual ~statement() = default;

    virtual void execute(scope &scp, env &ctx) const = 0;

    virtual std::string to_string() const = 0;
  };

  class expression_statement : public statement
  {
  public:
    expression_statement(std::unique_ptr<expression> &&expr) : expr(std::move(expr)) {}

    void execute(scope &scp, env &ctx) const override { expr->evaluate(scp, ctx); }

    std::string to_string() const override { return expr->to_string() + ";"; }

  private:
    std::unique_ptr<expression> expr;
  };
} // namespace riddle
