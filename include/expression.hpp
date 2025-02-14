#pragma once

#include "term.hpp"
#include "lexer.hpp"

namespace riddle
{
  class scope;

  class expression
  {
  public:
    expression() = default;
    expression(const expression &) = delete;
    virtual ~expression() = default;

    /**
     * @brief Evaluates the expression within the given scope and environment context.
     *
     * @param scp The scope in which the expression is evaluated.
     * @param ctx The environment context used during evaluation.
     * @return expr A shared pointer to the evaluated item.
     */
    [[nodiscard]] virtual expr evaluate(const scope &scp, env &ctx) const = 0;
  };

  class bool_expression final : public expression
  {
  public:
    bool_expression(bool_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    bool_token l;
  };

  class int_expression final : public expression
  {
  public:
    int_expression(int_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    int_token l;
  };

  class bounded_int_expression final : public expression
  {
  public:
    bounded_int_expression(int_token &&lb, int_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    int_token lb;
    int_token ub;
  };

  class uncertain_int_expression final : public expression
  {
  public:
    uncertain_int_expression(int_token &&lb, int_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    int_token lb;
    int_token ub;
  };

  class real_expression final : public expression
  {
  public:
    real_expression(real_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    real_token l;
  };

  class bounded_real_expression final : public expression
  {
  public:
    bounded_real_expression(real_token &&lb, real_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    real_token lb;
    real_token ub;
  };

  class uncertain_real_expression final : public expression
  {
  public:
    uncertain_real_expression(real_token &&lb, real_token &&ub) noexcept : lb(std::move(lb)), ub(std::move(ub)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    real_token lb;
    real_token ub;
  };

  class string_expression final : public expression
  {
  public:
    string_expression(string_token &&l) noexcept : l(std::move(l)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    string_token l;
  };

  class id_expression final : public expression
  {
  public:
    id_expression(std::vector<id_token> &&obj_id) noexcept : object_id(std::move(obj_id)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> object_id;
  };

  class and_expression final : public expression
  {
  public:
    and_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

    friend utils::u_ptr<expression> push_negations(utils::u_ptr<expression> expr) noexcept;
    friend utils::u_ptr<expression> distribute(utils::u_ptr<expression> expr) noexcept;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class or_expression final : public expression
  {
  public:
    or_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

    friend utils::u_ptr<expression> push_negations(utils::u_ptr<expression> expr) noexcept;
    friend utils::u_ptr<expression> distribute(utils::u_ptr<expression> expr) noexcept;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class xor_expression final : public expression
  {
  public:
    xor_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class not_expression final : public expression
  {
  public:
    not_expression(utils::u_ptr<expression> xpr) noexcept : xpr(std::move(xpr)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

    friend utils::u_ptr<expression> push_negations(utils::u_ptr<expression> expr) noexcept;

  private:
    utils::u_ptr<expression> xpr;
  };

  class minus_expression final : public expression
  {
  public:
    minus_expression(utils::u_ptr<expression> xpr) noexcept : xpr(std::move(xpr)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> xpr;
  };

  class sum_expression final : public expression
  {
  public:
    sum_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class subtraction_expression final : public expression
  {
  public:
    subtraction_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class product_expression final : public expression
  {
  public:
    product_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class division_expression final : public expression
  {
  public:
    division_expression(std::vector<utils::u_ptr<expression>> &&xprs) noexcept : xprs(std::move(xprs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<utils::u_ptr<expression>> xprs;
  };

  class lt_expression final : public expression
  {
  public:
    lt_expression(utils::u_ptr<expression> lhs, utils::u_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> lhs;
    utils::u_ptr<expression> rhs;
  };

  class le_expression final : public expression
  {
  public:
    le_expression(utils::u_ptr<expression> lhs, utils::u_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> lhs;
    utils::u_ptr<expression> rhs;
  };

  class gt_expression final : public expression
  {
  public:
    gt_expression(utils::u_ptr<expression> lhs, utils::u_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> lhs;
    utils::u_ptr<expression> rhs;
  };

  class ge_expression final : public expression
  {
  public:
    ge_expression(utils::u_ptr<expression> lhs, utils::u_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> lhs;
    utils::u_ptr<expression> rhs;
  };

  class eq_expression final : public expression
  {
  public:
    eq_expression(utils::u_ptr<expression> lhs, utils::u_ptr<expression> rhs) noexcept : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    utils::u_ptr<expression> lhs;
    utils::u_ptr<expression> rhs;
  };

  class constructor_expression final : public expression
  {
  public:
    constructor_expression(std::vector<id_token> &&tp_id, std::vector<utils::u_ptr<expression>> &&args) noexcept : type_id(std::move(tp_id)), arguments(std::move(args)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> type_id;
    std::vector<utils::u_ptr<expression>> arguments;
  };

  class call_expression final : public expression
  {
  public:
    call_expression(std::vector<id_token> &&obj_id, id_token &&fn_id, std::vector<utils::u_ptr<expression>> &&args) noexcept : object_id(std::move(obj_id)), function_id(std::move(fn_id)), arguments(std::move(args)) {}

    [[nodiscard]] expr evaluate(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> object_id;
    id_token function_id;
    std::vector<utils::u_ptr<expression>> arguments;
  };
} // namespace riddle
