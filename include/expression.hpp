#pragma once

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

    /**
     * @brief Evaluates the expression within the given scope and environment context.
     *
     * @param scp The scope in which the expression is evaluated.
     * @param ctx The environment context used during evaluation.
     * @return std::shared_ptr<item> A shared pointer to the evaluated item.
     */
    [[nodiscard]] virtual std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const = 0;
  };

  class bool_expression : public expression
  {
  public:
    bool_expression(std::unique_ptr<bool_token> l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<bool_token> l;
  };

  class int_expression : public expression
  {
  public:
    int_expression(std::unique_ptr<int_token> l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<int_token> l;
  };
} // namespace riddle
