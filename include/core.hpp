#pragma once

#include <vector>
#include "conjunction.hpp"

namespace riddle
{
  class component_type;
  class predicate;

  /**
   * @brief The core of the language.
   */
  class core : public scope, public env
  {
  public:
    core();
    virtual ~core() = default;

    /**
     * @brief Create a new bool expression.
     *
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> new_bool() = 0;
    /**
     * @brief Create a new bool expression with a value.
     *
     * @param value The value of the bool expression.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> new_bool(bool value) = 0;

    /**
     * @brief Create a new int expression.
     *
     * @return std::shared_ptr<item> The int expression.
     */
    virtual std::shared_ptr<item> new_int() = 0;
    /**
     * @brief Create a new int expression with a value.
     *
     * @param value The value of the int expression.
     * @return std::shared_ptr<item> The int expression.
     */
    virtual std::shared_ptr<item> new_int(INTEGER_TYPE value) = 0;

    /**
     * @brief Create a new real expression.
     *
     * @return std::shared_ptr<item> The real expression.
     */
    virtual std::shared_ptr<item> new_real() = 0;
    /**
     * @brief Create a new real expression with a value.
     *
     * @param value The value of the real expression.
     * @return std::shared_ptr<item> The real expression.
     */
    virtual std::shared_ptr<item> new_real(const utils::rational &value) = 0;

    /**
     * @brief Create a new time expression.
     *
     * @return std::shared_ptr<item> The time expression.
     */
    virtual std::shared_ptr<item> new_time() = 0;
    /**
     * @brief Create a new time expression with a value.
     *
     * @param value The value of the time expression.
     * @return std::shared_ptr<item> The time expression.
     */
    virtual std::shared_ptr<item> new_time(const utils::rational &value) = 0;

    /**
     * @brief Create a new string expression.
     *
     * @return std::shared_ptr<item> The string expression.
     */
    virtual std::shared_ptr<item> new_string() = 0;
    /**
     * @brief Create a new string expression with a value.
     *
     * @param value The value of the string expression.
     * @return std::shared_ptr<item> The string expression.
     */
    virtual std::shared_ptr<item> new_string(const std::string &value) = 0;

    /**
     * @brief Create a new item.
     *
     * @param tp The type of the item.
     * @return std::shared_ptr<item> The item.
     */
    virtual std::shared_ptr<item> new_item(component_type &tp);

    /**
     * @brief Create a new enum.
     *
     * @param tp The type of the enum.
     * @param values The values of the enum.
     * @return std::shared_ptr<item> The enum.
     */
    virtual std::shared_ptr<item> new_enum(type &tp, const std::vector<std::shared_ptr<item>> &values) = 0;

    /**
     * @brief Return the opposite of the numeric expression.
     *
     * @param expr The numeric expression.
     * @return std::shared_ptr<item> The opposite of the expression.
     */
    virtual std::shared_ptr<item> minus(const std::shared_ptr<item> &expr) = 0;

    /**
     * @brief Return the sum of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The numeric expressions.
     * @return std::shared_ptr<item> The sum of the numeric expressions.
     */
    virtual std::shared_ptr<item> add(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the difference of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The numeric expressions.
     * @return std::shared_ptr<item> The difference of the numeric expressions.
     */
    virtual std::shared_ptr<item> sub(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the product of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The numeric expressions.
     * @return std::shared_ptr<item> The product of the numeric expressions.
     */
    virtual std::shared_ptr<item> mul(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the quotient of the numeric expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The numeric expressions.
     * @return std::shared_ptr<item> The quotient of the numeric expressions.
     */
    virtual std::shared_ptr<item> div(const std::vector<std::shared_ptr<item>> &exprs) = 0;

    /**
     * @brief Return a bool expression that is true if the left hand side is less than the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> lt(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is less than or equal to the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> leq(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is greater than or equal to the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> geq(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;
    /**
     * @brief Return a bool expression that is true if the left hand side is greater than the right hand side.
     *
     * @param lhs The left hand side numeric expression.
     * @param rhs The right hand side numeric expression.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> gt(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;

    /**
     * @brief Return a bool expression that is true if the left hand side is equal to the right hand side.
     *
     * @param lhs The left hand side.
     * @param rhs The right hand side.
     * @return std::shared_ptr<item> The bool expression.
     */
    virtual std::shared_ptr<item> eq(const std::shared_ptr<item> &lhs, const std::shared_ptr<item> &rhs) = 0;

    /**
     * @brief Return the logical and of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The bool expressions.
     * @return std::shared_ptr<item> The logical and of the bool expressions.
     */
    virtual std::shared_ptr<item> conj(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the logical or of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The bool expressions.
     * @return std::shared_ptr<item> The logical or of the bool expressions.
     */
    virtual std::shared_ptr<item> disj(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the logical exclusive or of the bool expressions.
     *
     * @param std::vector<std::shared_ptr<item>> The bool expressions.
     * @return std::shared_ptr<item> The logical exclusive or of the bool expressions.
     */
    virtual std::shared_ptr<item> exct_one(const std::vector<std::shared_ptr<item>> &exprs) = 0;
    /**
     * @brief Return the logical not of the bool expression.
     *
     * @param expr The bool expression.
     * @return std::shared_ptr<item> The logical not of the bool expression.
     */
    virtual std::shared_ptr<item> negate(const std::shared_ptr<item> &expr) = 0;

    /**
     * @brief Assert a fact.
     *
     * @param fact The fact.
     */
    virtual void assert_fact(const std::shared_ptr<item> &fact) = 0;

    /**
     * @brief Create a new disjunction of conjunctions.
     *
     * @param disjuncts The disjuncts.
     */
    virtual void new_disjunction(const std::vector<std::unique_ptr<conjunction>> &disjuncts) = 0;

    /**
     * @brief Create a new fact.
     *
     * @param pred The predicate.
     * @param arguments The arguments.
     * @return std::shared_ptr<item> The fact.
     */
    virtual std::shared_ptr<item> new_fact(predicate &pred, const std::vector<std::pair<std::string, std::shared_ptr<item>>> &arguments) = 0;
    /**
     * @brief Create a new goal.
     *
     * @param pred The predicate.
     * @param arguments The arguments.
     * @return std::shared_ptr<item> The goal.
     */
    virtual std::shared_ptr<item> new_goal(predicate &pred, const std::vector<std::pair<std::string, std::shared_ptr<item>>> &arguments) = 0;

    /**
     * @brief Get a field by name.
     *
     * @param name The name of the field.
     * @return std::optional<std::reference_wrapper<field>> The field.
     */
    virtual std::optional<std::reference_wrapper<field>> get_field(const std::string &name) const noexcept override;

    const type &get_bool_type() const { return *types.at("bool"); }
    const type &get_int_type() const { return *types.at("int"); }
    const type &get_real_type() const { return *types.at("real"); }
    const type &get_time_type() const { return *types.at("time"); }
    const type &get_string_type() const { return *types.at("string"); }

    /**
     * @brief Get an item by name.
     *
     * @param name The name of the item.
     * @return std::shared_ptr<item> The item.
     */
    virtual std::shared_ptr<item> get(const std::string &name) const noexcept override;

  private:
    std::map<std::string, std::unique_ptr<type>> types;
  };
} // namespace riddle
