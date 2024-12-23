#pragma once

#include "item.hpp"
#include "lexer.hpp"
#include <vector>

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

  class bool_expression final : public expression
  {
  public:
    bool_expression(bool_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    bool_token l;
  };

  class int_expression final : public expression
  {
  public:
    int_expression(int_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    int_token l;
  };

  class uncertain_int_expression final : public expression
  {
  public:
    uncertain_int_expression(int_token &&lb, int_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    int_token lb;
    int_token ub;
  };

  class real_expression final : public expression
  {
  public:
    real_expression(real_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    real_token l;
  };

  class uncertain_real_expression final : public expression
  {
  public:
    uncertain_real_expression(real_token &&lb, real_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    real_token lb;
    real_token ub;
  };

  class string_expression final : public expression
  {
  public:
    string_expression(string_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    string_token l;
  };

  class id_expression final : public expression
  {
  public:
    id_expression(std::vector<id_token> &&obj_id) noexcept : object_id(std::move(obj_id)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> object_id;
  };

  class and_expression final : public expression
  {
  public:
    and_expression(std::vector<std::unique_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<expression>> xprs;
  };

  class or_expression final : public expression
  {
  public:
    or_expression(std::vector<std::unique_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<expression>> xprs;
  };

  class xor_expression final : public expression
  {
  public:
    xor_expression(std::vector<std::unique_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<expression>> xprs;
  };

  class not_expression final : public expression
  {
  public:
    not_expression(std::unique_ptr<expression> xpr) noexcept : xpr(std::move(xpr)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> xpr;
  };

  class implies_expression final : public expression
  {
  public:
    implies_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class plus_expression final : public expression
  {
  public:
    plus_expression(std::unique_ptr<expression> xpr) noexcept : xpr(std::move(xpr)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> xpr;
  };

  class minus_expression final : public expression
  {
  public:
    minus_expression(std::unique_ptr<expression> xpr) noexcept : xpr(std::move(xpr)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> xpr;
  };

  class sum_expression final : public expression
  {
  public:
    sum_expression(std::vector<std::unique_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<expression>> xprs;
  };

  class product_expression final : public expression
  {
  public:
    product_expression(std::vector<std::unique_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<expression>> xprs;
  };

  class divide_expression final : public expression
  {
  public:
    divide_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class lt_expression final : public expression
  {
  public:
    lt_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class le_expression final : public expression
  {
  public:
    le_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class gt_expression final : public expression
  {
  public:
    gt_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class ge_expression final : public expression
  {
  public:
    ge_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class eq_expression final : public expression
  {
  public:
    eq_expression(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> lhs;
    std::unique_ptr<expression> rhs;
  };

  class constructor_expression final : public expression
  {
  public:
    constructor_expression(std::vector<id_token> &&tp_id, std::vector<std::unique_ptr<expression>> &&args) noexcept : type_id(std::move(tp_id)), arguments(std::move(args)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> type_id;
    std::vector<std::unique_ptr<expression>> arguments;
  };

  class call_expression final : public expression
  {
  public:
    call_expression(std::vector<id_token> &&obj_id, id_token &&fn_id, std::vector<std::unique_ptr<expression>> &&args) noexcept : object_id(std::move(obj_id)), function_id(std::move(fn_id)), arguments(std::move(args)) {}

    [[nodiscard]] std::shared_ptr<item> evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> object_id;
    id_token function_id;
    std::vector<std::unique_ptr<expression>> arguments;
  };
} // namespace riddle
