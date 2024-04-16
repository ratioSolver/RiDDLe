#pragma once

#include <vector>
#include "conjunction.hpp"
#include "bool.hpp"
#include "type.hpp"
#include "inf_rational.hpp"
#include "enum.hpp"
#include "compilation_unit.hpp"

#ifdef ENABLE_VISUALIZATION
#include <unordered_map>
#endif

namespace riddle
{
  class component_type;
  class predicate;
  class typedef_declaration;
  class enum_declaration;
  class method_declaration;
  class class_declaration;
  class predicate_declaration;

  /**
   * @brief The core of the language.
   */
  class core : public scope, public env, public std::enable_shared_from_this<core>
  {
    friend class typedef_declaration;
    friend class enum_declaration;
    friend class method_declaration;
    friend class class_declaration;
    friend class predicate_declaration;
    friend class enum_item;

  public:
    core();
    virtual ~core() = default;

    /**
     * @brief Read a RiDDLe script.
     *
     * @param script The RiDDLe script.
     */
    virtual void read(const std::string &script);
    /**
     * @brief Read a list of RiDDLe files.
     *
     * @param files The list of RiDDLe files.
     */
    virtual void read(const std::vector<std::string> &files);

    /**
     * @brief Create a new bool expression.
     *
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_bool() = 0;
    /**
     * @brief Create a new bool expression with a value.
     *
     * @param value The value of the bool expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] std::shared_ptr<bool_item> new_bool(bool value);

    /**
     * @brief Create a new int expression.
     *
     * @return std::shared_ptr<arith_item> The int expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_int() = 0;
    /**
     * @brief Create a new int expression with a value.
     *
     * @param value The value of the int expression.
     * @return std::shared_ptr<arith_item> The int expression.
     */
    [[nodiscard]] std::shared_ptr<arith_item> new_int(INTEGER_TYPE value);

    /**
     * @brief Create a new real expression.
     *
     * @return std::shared_ptr<arith_item> The real expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_real() = 0;
    /**
     * @brief Create a new real expression with a value.
     *
     * @param value The value of the real expression.
     * @return std::shared_ptr<arith_item> The real expression.
     */
    [[nodiscard]] std::shared_ptr<arith_item> new_real(const utils::rational &value);

    /**
     * @brief Create a new time expression.
     *
     * @return std::shared_ptr<arith_item> The time expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_time() = 0;
    /**
     * @brief Create a new time expression with a value.
     *
     * @param value The value of the time expression.
     * @return std::shared_ptr<arith_item> The time expression.
     */
    [[nodiscard]] std::shared_ptr<arith_item> new_time(const utils::rational &value);

    /**
     * @brief Create a new string expression.
     *
     * @return std::shared_ptr<string_item> The string expression.
     */
    [[nodiscard]] std::shared_ptr<string_item> new_string();
    /**
     * @brief Create a new string expression with a value.
     *
     * @param value The value of the string expression.
     * @return std::shared_ptr<string_item> The string expression.
     */
    [[nodiscard]] std::shared_ptr<string_item> new_string(const std::string &value);

    /**
     * @brief Create a new item.
     *
     * @param tp The type of the item.
     * @return std::shared_ptr<component> The item.
     */
    [[nodiscard]] virtual std::shared_ptr<component> new_item(component_type &tp);

    /**
     * @brief Create a new enum.
     *
     * @param tp The type of the enum.
     * @param values The values of the enum.
     * @return std::shared_ptr<enum_item> The enum.
     */
    [[nodiscard]] virtual std::shared_ptr<enum_item> new_enum(type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) = 0;

    /**
     * @brief Return the opposite of the numeric expression.
     *
     * @param expr The numeric expression.
     * @return std::shared_ptr<arith_item> The opposite of the expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> minus(const std::shared_ptr<arith_item> &expr) = 0;

    /**
     * @brief Return the sum of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<arith_item>> The numeric expressions.
     * @return std::shared_ptr<arith_item> The sum of the numeric expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> add(const std::vector<std::shared_ptr<arith_item>> &exprs) = 0;
    /**
     * @brief Return the difference of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<arith_item>> The numeric expressions.
     * @return std::shared_ptr<arith_item> The difference of the numeric expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> sub(const std::vector<std::shared_ptr<arith_item>> &exprs) = 0;
    /**
     * @brief Return the product of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<arith_item>> The numeric expressions.
     * @return std::shared_ptr<arith_item> The product of the numeric expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> mul(const std::vector<std::shared_ptr<arith_item>> &exprs) = 0;
    /**
     * @brief Return the quotient of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<arith_item>> The numeric expressions.
     * @return std::shared_ptr<arith_item> The quotient of the numeric expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> div(const std::vector<std::shared_ptr<arith_item>> &exprs) = 0;

    /**
     * @brief Return a bool expression that is true if the left hand side is less than the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> lt(const std::shared_ptr<arith_item> &lhs, const std::shared_ptr<arith_item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is less than or equal to the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> leq(const std::shared_ptr<arith_item> &lhs, const std::shared_ptr<arith_item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is greater than or equal to the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> geq(const std::shared_ptr<arith_item> &lhs, const std::shared_ptr<arith_item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is greater than the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> gt(const std::shared_ptr<arith_item> &lhs, const std::shared_ptr<arith_item> &rhs) = 0;

    /**
     * @brief Return a bool expression that is true if the left hand side is equal to the right hand side.
     *
     * @param lhs The left hand side.
     * @param rhs The right hand side.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> eq(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;

    /**
     * @brief Return the logical and of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<bool_item>> The bool expressions.
     * @return std::shared_ptr<bool_item> The logical and of the bool expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> conj(const std::vector<std::shared_ptr<bool_item>> &exprs) = 0;
    /**
     * @brief Return the logical or of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<bool_item>> The bool expressions.
     * @return std::shared_ptr<bool_item> The logical or of the bool expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> disj(const std::vector<std::shared_ptr<bool_item>> &exprs) = 0;
    /**
     * @brief Return the logical exclusive or of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<bool_item>> The bool expressions.
     * @return std::shared_ptr<bool_item> The logical exclusive or of the bool expressions.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> exct_one(const std::vector<std::shared_ptr<bool_item>> &exprs) = 0;
    /**
     * @brief Return the logical not of the bool expression.
     *
     * @param expr The bool expression.
     * @return std::shared_ptr<bool_item> The logical not of the bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> negate(const std::shared_ptr<bool_item> &expr) = 0;

    /**
     * @brief Assert a fact.
     *
     * @param fact The fact.
     */
    virtual void assert_fact(const std::shared_ptr<bool_item> &fact) = 0;

    /**
     * @brief Create a new disjunction of conjunctions.
     *
     * @param disjuncts The disjuncts.
     */
    virtual void new_disjunction(std::vector<std::unique_ptr<conjunction>> &&disjuncts) = 0;

    /**
     * @brief Create a new fact atom with the given predicate and arguments.
     *
     * @param pred The predicate.
     * @param arguments The arguments.
     * @return std::shared_ptr<atom> The atom.
     */
    [[nodiscard]] virtual std::shared_ptr<atom> new_fact(predicate &pred, std::map<std::string, std::shared_ptr<item>> &&arguments = {});
    /**
     * @brief Create a new goal atom with the given predicate and arguments.
     *
     * @param pred The predicate.
     * @param arguments The arguments.
     * @return std::shared_ptr<atom> The atom.
     */
    [[nodiscard]] virtual std::shared_ptr<atom> new_goal(predicate &pred, std::map<std::string, std::shared_ptr<item>> &&arguments = {});

    /**
     * @brief Get the bool value of the expression.
     *
     * @param expr The expression.
     * @return utils::lbool The bool value.
     */
    [[nodiscard]] virtual utils::lbool bool_value(const bool_item &expr) const noexcept = 0;

    /**
     * @brief Get the arithmetic value of the expression.
     *
     * @param expr The expression.
     * @return utils::inf_rational The arithmetic value.
     */
    [[nodiscard]] virtual utils::inf_rational arithmetic_value(const arith_item &expr) const noexcept = 0;

    /**
     * @brief Get the bounds of the arithmetic expression.
     *
     * @param expr The expression.
     * @return std::pair<utils::inf_rational, utils::inf_rational> The bounds.
     */
    [[nodiscard]] virtual std::pair<utils::inf_rational, utils::inf_rational> bounds(const arith_item &expr) const noexcept = 0;

    /**
     * @brief Get the domain of the expression.
     *
     * @param expr The expression.
     * @return std::vector<std::reference_wrapper<utils::enum_val>> The domain.
     */
    [[nodiscard]] virtual std::vector<std::reference_wrapper<utils::enum_val>> domain(const enum_item &expr) const noexcept = 0;

    /**
     * @brief Assign a value to the expression.
     *
     * @param expr The expression.
     * @param val The value to assign.
     * @return true If the assignment is successful.
     * @return false If the assignment is not successful.
     */
    virtual bool assign(const enum_item &expr, utils::enum_val &val) = 0;

    /**
     * @brief Remove a value from the domain of the expression.
     *
     * @param expr The expression.
     * @param val The value to remove.
     */
    virtual void forbid(const enum_item &expr, utils::enum_val &val) = 0;

    /**
     * @brief Get a field by name.
     *
     * @param name The name of the field.
     * @return std::optional<std::reference_wrapper<field>> The field.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<field>> get_field(const std::string &name) const noexcept override;

    [[nodiscard]] std::optional<std::reference_wrapper<method>> get_method(const std::string &name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;

    [[nodiscard]] std::optional<std::reference_wrapper<type>> get_type(const std::string &name) const override;
    [[nodiscard]] std::optional<std::reference_wrapper<predicate>> get_predicate(const std::string &name) const override;

    [[nodiscard]] bool_type &get_bool_type() const { return bool_tp; }
    [[nodiscard]] int_type &get_int_type() const { return int_tp; }
    [[nodiscard]] real_type &get_real_type() const { return real_tp; }
    [[nodiscard]] time_type &get_time_type() const { return time_tp; }
    [[nodiscard]] string_type &get_string_type() const { return string_tp; }

    /**
     * @brief Get an item by name.
     *
     * @param name The name of the item.
     * @return std::shared_ptr<item> The item.
     */
    [[nodiscard]] virtual std::shared_ptr<item> get(const std::string &name) override;

  private:
    /**
     * @brief Create a new fact or goal atom with the given predicate and arguments.
     *
     * @param is_fact Whether the atom is a fact or a goal.
     * @param pred The predicate.
     * @param arguments The arguments.
     * @return std::shared_ptr<atom> The atom.
     */
    [[nodiscard]] virtual std::shared_ptr<atom> new_atom(bool is_fact, predicate &pred, std::map<std::string, std::shared_ptr<item>> &&arguments = {}) = 0;

    /**
     * @brief Returns the expression with the given name in the given enum.
     *
     * @param enm The enum.
     * @param name The name of the expression.
     * @return std::shared_ptr<item> The expression.
     */
    [[nodiscard]] virtual std::shared_ptr<item> get(enum_item &enm, const std::string &name) = 0;

  private:
    void add_type(std::unique_ptr<type> &&tp);
    void add_predicate(std::unique_ptr<predicate> &&pred);
    void add_method(std::unique_ptr<method> &&mthd);

#ifdef ENABLE_VISUALIZATION
  public:
    std::string guess_name(const item &itm) const noexcept
    {
      if (const auto at_f = expr_names.find(&itm); at_f != expr_names.cend())
        return at_f->second;
      return "";
    }

  private:
    void recompute_names() noexcept;

    std::unordered_map<const item *, const std::string> expr_names;
#endif

  private:
    std::map<std::string, std::unique_ptr<type>> types;                  // the types..
    std::map<std::string, std::unique_ptr<predicate>> predicates;        // the predicates..
    std::map<std::string, std::vector<std::unique_ptr<method>>> methods; // the methods..
    bool_type &bool_tp;                                                  // the bool type..
    int_type &int_tp;                                                    // the int type..
    real_type &real_tp;                                                  // the real type..
    time_type &time_tp;                                                  // the time type..
    string_type &string_tp;                                              // the string type..
    std::vector<std::unique_ptr<compilation_unit>> cus;                  // the compilation units..
  };

  /**
   * @brief Determine if the expression is a boolean.
   *
   * @param x The expression.
   * @return true If the expression is a boolean.
   * @return false If the expression is not a boolean.
   */
  [[nodiscard]] inline bool is_bool(const item &x) noexcept { return &x.get_type().get_scope().get_core().get_bool_type() == &x.get_type(); }
  /**
   * @brief Determine if the expression is an integer.
   *
   * @param x The expression.
   * @return true If the expression is an integer.
   * @return false If the expression is not an integer.
   */
  [[nodiscard]] inline bool is_int(const item &x) noexcept { return &x.get_type().get_scope().get_core().get_int_type() == &x.get_type(); }
  /**
   * @brief Determine if the expression is a real.
   *
   * @param x The expression.
   * @return true If the expression is a real.
   * @return false If the expression is not a real.
   */
  [[nodiscard]] inline bool is_real(const item &x) noexcept { return &x.get_type().get_scope().get_core().get_real_type() == &x.get_type(); }
  /**
   * @brief Determine if the expression is a time.
   *
   * @param x The expression.
   * @return true If the expression is a time.
   * @return false If the expression is not a time.
   */
  [[nodiscard]] inline bool is_time(const item &x) noexcept { return &x.get_type().get_scope().get_core().get_time_type() == &x.get_type(); }
  /**
   * @brief Determine if the expression is an arithmetic value.
   *
   * @param x The expression.
   * @return true If the expression is an arithmetic value.
   * @return false If the expression is not an arithmetic value.
   */
  [[nodiscard]] inline bool is_arith(const item &x) noexcept { return is_int(x) || is_real(x) || is_time(x); }
  /**
   * @brief Determine if the expression is a string.
   *
   * @param x The expression.
   * @return true If the expression is a string.
   * @return false If the expression is not a string.
   */
  [[nodiscard]] inline bool is_string(const item &x) noexcept { return &x.get_type().get_scope().get_core().get_string_type() == &x.get_type(); }
  /**
   * @brief Determine if the expression is an enumeration.
   *
   * @param x The expression.
   * @return true If the expression is an enumeration.
   * @return false If the expression is not an enumeration.
   */
  [[nodiscard]] inline bool is_enum(const item &x) noexcept { return dynamic_cast<const enum_item *>(&x) != nullptr; }
  /**
   * @brief Determine if the expression is a constant.
   *
   * A constant is an expression that has a single value.
   * For example, a boolean expression with a value of true or false.
   * An arithmetic expression with a lower bound equal to the upper bound.
   * An enumeration expression with a single value.
   *
   * @param x The expression.
   * @return true If the expression is a constant.
   * @return false If the expression is not a constant.
   */
  [[nodiscard]] inline bool is_constant(const item &x) noexcept
  {
    if (is_bool(x)) // the expression is a boolean..
      return x.get_type().get_scope().get_core().bool_value(static_cast<const bool_item &>(x)) != utils::Undefined;
    else if (is_arith(x))
    { // the expression is an arithmetic value..
      auto [lb, ub] = x.get_type().get_scope().get_core().bounds(static_cast<const arith_item &>(x));
      return lb == ub;
    }
    else if (is_enum(x)) // the expression is an enumeration value..
      return x.get_type().get_scope().get_core().domain(static_cast<const enum_item &>(x)).size() == 1;
    else // the expression is a single value..
      return true;
  }

  [[nodiscard]] type &determine_type(const std::vector<std::shared_ptr<arith_item>> &xprs);

  inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }

  class unsolvable_exception : public std::exception
  {
    const char *what() const noexcept override { return "the problem is unsolvable.."; }
  };
} // namespace riddle
