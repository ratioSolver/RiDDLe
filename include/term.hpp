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
  using bool_expr = utils::s_ptr<bool_term>;
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
    [[nodiscard]] type &get_type() const { return tp; }

    [[nodiscard]] virtual bool_expr operator==(expr rhs) const;

    [[nodiscard]] virtual json::json to_json() const = 0;

  private:
    type &tp;
  };

  class bool_term : public term
  {
  public:
    bool_term(bool_type &tp);

    [[nodiscard]] virtual json::json to_json() const override;
  };

  class bool_and : public bool_term
  {
    friend expression_statement;

  public:
    bool_and(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class bool_or : public bool_term
  {
    friend expression_statement;

  public:
    bool_or(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class bool_xor : public bool_term
  {
  public:
    bool_xor(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_term(tp), args(std::move(args)) {}

  private:
    std::vector<bool_expr> args;
  };

  class bool_not : public bool_term
  {
  public:
    bool_not(bool_type &tp, bool_expr arg) noexcept : bool_term(tp), arg(std::move(arg)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;

  private:
    bool_expr arg;
  };

  class arith_term : public term
  {
  public:
    arith_term(int_type &tp);
    arith_term(real_type &tp);
    arith_term(time_type &tp);

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using arith_expr = utils::s_ptr<arith_term>;

  class string_term : public term
  {
  public:
    string_term(string_type &tp);
    virtual ~string_term() = default;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using string_expr = utils::s_ptr<string_term>;

  class enum_itm : public term
  {
  public:
    enum_itm(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values);

    [[nodiscard]] const std::vector<utils::ref_wrapper<utils::enum_val>> &get_values() const noexcept { return values; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    std::vector<utils::ref_wrapper<utils::enum_val>> values;
  };

  using enum_expr = utils::s_ptr<enum_itm>;

  class component : public term, public env
  {
  public:
    component(component_type &tp);

    [[nodiscard]] bool_expr operator==(expr rhs) const override;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  enum atom_state
  {
    active,   // the atom is active
    inactive, // the atom is inactive
    unified   // the atom is unified
  };

  class atom_term : public term, public env
  {
  public:
    atom_term(predicate &t, bool fact, std::map<std::string, expr, std::less<>> &&args = {});

    [[nodiscard]] bool is_fact() const { return fact; }

    [[nodiscard]] virtual atom_state get_state() const { return atom_state::active; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    static env &atom_parent(const predicate &t, const std::map<std::string, expr, std::less<>> &args);

  private:
    bool fact; // whether the atom is a fact
  };

  using atom_expr = utils::s_ptr<atom_term>;

  [[nodiscard]] bool_expr push_negations(bool_expr expr) noexcept;
  [[nodiscard]] bool_expr distribute(bool_expr expr) noexcept;
  [[nodiscard]] inline bool_expr to_cnf(bool_expr expr) noexcept { return distribute(push_negations(expr)); }
} // namespace riddle
