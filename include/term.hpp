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
  class bool_itm;
  using bool_expr = utils::s_ptr<bool_itm>;
  class expression_statement;

  /**
   * @class item item.hpp "include/item.hpp"
   * @brief The item class.
   *
   * The item class is the base class for all items in the RiDDLe language. An item
   * is a named entity that has a type. Items are stored in environments and can be
   * retrieved by their name.
   */
  class item : public utils::enum_val
  {
  public:
    item(type &tp) noexcept : tp(tp) {}
    item(const item &) = delete;

    /**
     * @brief Get the type of the item.
     *
     * This function returns a reference to the type of the item.
     *
     * @return A reference to the type of the item.
     */
    [[nodiscard]] type &get_type() const { return tp; }

    [[nodiscard]] virtual bool_expr operator==(expr rhs) const;

    [[nodiscard]] virtual json::json to_json() const = 0;

    /**
     * @brief Get the unique identifier of the item.
     *
     * This function returns the unique identifier of the item.
     *
     * @return The unique identifier of the item.
     */
    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

  private:
    type &tp;
  };

  class bool_itm : public item
  {
  public:
    bool_itm(bool_type &tp);

    [[nodiscard]] virtual json::json to_json() const override;
  };

  class bool_and : public bool_itm
  {
    friend expression_statement;

  public:
    bool_and(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_itm(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class bool_or : public bool_itm
  {
    friend expression_statement;

  public:
    bool_or(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_itm(tp), args(std::move(args)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;
    friend bool_expr distribute(bool_expr expr) noexcept;

  private:
    std::vector<bool_expr> args;
  };

  class bool_xor : public bool_itm
  {
  public:
    bool_xor(bool_type &tp, std::vector<bool_expr> &&args) noexcept : bool_itm(tp), args(std::move(args)) {}

  private:
    std::vector<bool_expr> args;
  };

  class bool_not : public bool_itm
  {
  public:
    bool_not(bool_type &tp, bool_expr arg) noexcept : bool_itm(tp), arg(std::move(arg)) {}

    friend bool_expr push_negations(bool_expr expr) noexcept;

  private:
    bool_expr arg;
  };

  class arith_itm : public item
  {
  public:
    arith_itm(int_type &tp);
    arith_itm(real_type &tp);
    arith_itm(time_type &tp);

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using arith_expr = utils::s_ptr<arith_itm>;

  class string_itm : public item
  {
  public:
    string_itm(string_type &tp);
    virtual ~string_itm() = default;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using string_expr = utils::s_ptr<string_itm>;

  class enum_itm : public item
  {
  public:
    enum_itm(type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values);

    [[nodiscard]] const std::vector<utils::ref_wrapper<utils::enum_val>> &get_values() const noexcept { return values; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    std::vector<utils::ref_wrapper<utils::enum_val>> values;
  };

  using enum_expr = utils::s_ptr<enum_itm>;

  class component : public item, public env
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

  class atm : public item, public env
  {
  public:
    atm(predicate &t, bool fact, std::map<std::string, expr, std::less<>> &&args = {});

    [[nodiscard]] bool is_fact() const { return fact; }

    [[nodiscard]] virtual atom_state get_state() const { return atom_state::active; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    static env &atom_parent(const predicate &t, const std::map<std::string, expr, std::less<>> &args);

  private:
    bool fact; // whether the atom is a fact
  };

  using atom_expr = utils::s_ptr<atm>;

  [[nodiscard]] bool_expr push_negations(bool_expr expr) noexcept;
  [[nodiscard]] bool_expr distribute(bool_expr expr) noexcept;
  [[nodiscard]] inline bool_expr to_cnf(bool_expr expr) noexcept { return distribute(push_negations(expr)); }
} // namespace riddle
