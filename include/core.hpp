#pragma once

#include <vector>
#include "conjunction.hpp"
#include "bool.hpp"
#include "type.hpp"
#include "inf_rational.hpp"
#include "enum.hpp"
#include "compilation_unit.hpp"

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
  class core : public scope, public env
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
    [[nodiscard]] virtual std::shared_ptr<string_item> new_string() = 0;
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
    virtual void new_disjunction(std::vector<std::unique_ptr<riddle::conjunction>> &&disjuncts) = 0;

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
     * @brief Check if the expression is constant.
     *
     * @param expr The expression.
     * @return true If the expression is constant.
     * @return false If the expression is not constant.
     */
    [[nodiscard]] virtual bool is_constant(const item &expr) const noexcept = 0;

    /**
     * @brief Get the bool value of the expression.
     *
     * @param expr The expression.
     * @return utils::lbool The bool value.
     */
    [[nodiscard]] virtual utils::lbool bool_value(const item &expr) const noexcept = 0;

    /**
     * @brief Get the arithmetic value of the expression.
     *
     * @param expr The expression.
     * @return utils::inf_rational The arithmetic value.
     */
    [[nodiscard]] virtual utils::inf_rational arithmetic_value(const item &expr) const noexcept = 0;

    /**
     * @brief Get the bounds of the arithmetic expression.
     *
     * @param expr The expression.
     * @return std::pair<utils::inf_rational, utils::inf_rational> The bounds.
     */
    [[nodiscard]] virtual std::pair<utils::inf_rational, utils::inf_rational> bounds(const item &expr) const noexcept = 0;

    /**
     * @brief Check if the expression is an enum.
     *
     * @param expr The expression.
     * @return true If the expression is an enum.
     * @return false If the expression is not an enum.
     */
    [[nodiscard]] virtual bool is_enum(const item &expr) const noexcept = 0;

    /**
     * @brief Get the domain of the expression.
     *
     * @param expr The expression.
     * @return std::vector<std::reference_wrapper<utils::enum_val>> The domain.
     */
    [[nodiscard]] virtual std::vector<std::reference_wrapper<utils::enum_val>> domain(const item &expr) const noexcept = 0;

    /**
     * @brief Assign a value to the expression.
     *
     * @param expr The expression.
     * @param val The value to assign.
     * @return true If the assignment is successful.
     * @return false If the assignment is not successful.
     */
    virtual bool assign(const item &expr, utils::enum_val &val) = 0;

    /**
     * @brief Remove a value from the domain of the expression.
     *
     * @param expr The expression.
     * @param val The value to remove.
     */
    virtual void forbid(const item &expr, utils::enum_val &val) = 0;

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

  [[nodiscard]] bool is_bool(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_bool_type() == &x.get_type(); }
  [[nodiscard]] bool is_int(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_int_type() == &x.get_type(); }
  [[nodiscard]] bool is_real(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_real_type() == &x.get_type(); }
  [[nodiscard]] bool is_time(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_time_type() == &x.get_type(); }
  [[nodiscard]] inline bool is_arith(const riddle::item &x) noexcept { return is_int(x) || is_real(x) || is_time(x); }
  [[nodiscard]] inline bool is_string(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_string_type() == &x.get_type(); }

  [[nodiscard]] type &determine_type(const std::vector<std::shared_ptr<item>> &xprs);

  inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }

  class unsolvable_exception : public std::exception
  {
    const char *what() const noexcept override { return "the problem is unsolvable.."; }
  };
} // namespace riddle
