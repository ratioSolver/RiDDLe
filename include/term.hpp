#pragma once

#include "env.hpp"
#include "enum.hpp"
#include <vector>

namespace riddle
{
  class type;
  class bool_type;
  class int_type;
  class real_type;
  class time_type;
  class string_type;
  class enum_type;
  class component_type;
  class predicate;
  class bool_term;
  using bool_expr = std::shared_ptr<bool_term>;
  class expression_statement;

  /**
   * @class term term.hpp "include/term.hpp"
   * @brief The term class.
   *
   * The term class is the base class for all terms in the RiDDLe language. A term
   * is a named entity that has a type. Terms are stored in environments and can be
   * retrieved by their name.
   */
  class term : public utils::enum_val
  {
  public:
    term(type &tp) noexcept : tp(tp) {}
    term(const term &) = delete;

    /**
     * @brief Get the unique identifier of the term.
     *
     * This function returns the unique identifier of the term.
     *
     * @return The unique identifier of the term.
     */
    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    /**
     * @brief Get the type of the term.
     *
     * This function returns a reference to the type of the term.
     *
     * @return A reference to the type of the term.
     */
    [[nodiscard]] type &get_type() const noexcept { return tp; }

    [[nodiscard]] virtual json::json to_json() const noexcept = 0;

  private:
    type &tp;
  };

  class bool_term : public term
  {
  public:
    bool_term(bool_type &tp) noexcept;

    [[nodiscard]] virtual json::json to_json() const noexcept override;
  };

  class and_term : public bool_term
  {
    friend expression_statement;

  public:
    and_term(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class or_term : public bool_term
  {
    friend expression_statement;

  public:
    or_term(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class xor_term : public bool_term
  {
  public:
    xor_term(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

  private:
    std::vector<bool_expr> args;
  };

  class bool_not : public bool_term
  {
  public:
    bool_not(bool_type &tp, bool_expr arg) noexcept : bool_term(tp), arg(std::move(arg)) {}

    bool_expr get_arg() const noexcept { return arg; }

    friend bool_expr push_negations(bool_expr expr) noexcept;

  private:
    bool_expr arg;
  };

  class arith_term : public term
  {
  public:
    arith_term(int_type &tp) noexcept;
    arith_term(real_type &tp) noexcept;
    arith_term(time_type &tp) noexcept;

    [[nodiscard]] virtual json::json to_json() const noexcept override;
  };

  using arith_expr = std::shared_ptr<arith_term>;

  class lt_term : public bool_term
  {
  public:
    lt_term(bool_type &tp, arith_expr lhs, arith_expr rhs) noexcept : bool_term(tp), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    arith_expr get_lhs() const noexcept { return lhs; }
    arith_expr get_rhs() const noexcept { return rhs; }

  private:
    arith_expr lhs, rhs;
  };

  class le_term : public bool_term
  {
  public:
    le_term(bool_type &tp, arith_expr lhs, arith_expr rhs) noexcept : bool_term(tp), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    arith_expr get_lhs() const noexcept { return lhs; }
    arith_expr get_rhs() const noexcept { return rhs; }

  private:
    arith_expr lhs, rhs;
  };

  class eq_term : public bool_term
  {
  public:
    eq_term(bool_type &tp, expr lhs, expr rhs) noexcept : bool_term(tp), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    expr get_lhs() const noexcept { return lhs; }
    expr get_rhs() const noexcept { return rhs; }

  private:
    expr lhs, rhs;
  };

  class ge_term : public bool_term
  {
  public:
    ge_term(bool_type &tp, arith_expr lhs, arith_expr rhs) noexcept : bool_term(tp), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    arith_expr get_lhs() const noexcept { return lhs; }
    arith_expr get_rhs() const noexcept { return rhs; }

  private:
    arith_expr lhs, rhs;
  };

  class gt_term : public bool_term
  {
  public:
    gt_term(bool_type &tp, arith_expr lhs, arith_expr rhs) noexcept : bool_term(tp), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    arith_expr get_lhs() const noexcept { return lhs; }
    arith_expr get_rhs() const noexcept { return rhs; }

  private:
    arith_expr lhs, rhs;
  };

  class string_term : public term
  {
  public:
    string_term(string_type &tp) noexcept;
    virtual ~string_term() = default;

    [[nodiscard]] virtual json::json to_json() const noexcept override;
  };

  using string_expr = std::shared_ptr<string_term>;

  class component : public term, public env
  {
  public:
    component(component_type &tp) noexcept;

    [[nodiscard]] virtual json::json to_json() const noexcept override;
  };

  class enum_term : public term, public env
  {
  public:
    enum_term(component_type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) noexcept;

    [[nodiscard]] const std::vector<std::reference_wrapper<utils::enum_val>> &get_values() const noexcept { return values; }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    std::vector<std::reference_wrapper<utils::enum_val>> values;
  };

  using enum_expr = std::shared_ptr<enum_term>;

  enum atom_state
  {
    active,   // the atom is active
    inactive, // the atom is inactive
    unified   // the atom is unified
  };

  class atom_term : public term, public env
  {
  public:
    atom_term(predicate &t, bool fact, std::map<std::string, expr, std::less<>> &&args = {}) noexcept;

    [[nodiscard]] bool is_fact() const { return fact; }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    static env &atom_parent(const predicate &t, const std::map<std::string, expr, std::less<>> &args);

  private:
    bool fact; // whether the atom is a fact
  };

  using atom_expr = std::shared_ptr<atom_term>;

  [[nodiscard]] bool_expr push_negations(bool_expr expr) noexcept;
  [[nodiscard]] bool_expr distribute(bool_expr expr) noexcept;
  [[nodiscard]] inline bool_expr to_cnf(bool_expr expr) noexcept { return distribute(push_negations(expr)); }

  [[nodiscard]] bool is_bool(expr xpr) noexcept;
  [[nodiscard]] bool is_int(expr xpr) noexcept;
  [[nodiscard]] bool is_real(expr xpr) noexcept;
  [[nodiscard]] bool is_time(expr xpr) noexcept;
} // namespace riddle
