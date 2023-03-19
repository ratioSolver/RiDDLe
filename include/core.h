#pragma once

#include "type.h"
#include "method.h"
#include "bool.h"
#include "inf_rational.h"

#ifdef BUILD_LISTENERS
#define FIRE_LOG(msg) fire_log(msg)
#define FIRE_READ(rddl) fire_read(rddl)
#define FIRE_STATE_CHANGED() fire_state_changed()
#define FIRE_STARTED_SOLVING() fire_started_solving()
#define FIRE_SOLUTION_FOUND() fire_solution_found()
#define FIRE_INCONSISTENT_PROBLEM() fire_inconsistent_problem()
#else
#define FIRE_LOG(msg)
#define FIRE_READ(rddl)
#define FIRE_STATE_CHANGED()
#define FIRE_STARTED_SOLVING()
#define FIRE_SOLUTION_FOUND()
#define FIRE_INCONSISTENT_PROBLEM()
#endif

#ifdef COMPUTE_NAMES
#define RECOMPUTE_NAMES() recompute_names()
#else
#define RECOMPUTE_NAMES()
#endif

namespace riddle
{
#ifdef BUILD_LISTENERS
  class core_listener;
#endif
  class core : public scope, public env
  {
    friend class ast::method_declaration;
    friend class ast::predicate_declaration;
    friend class ast::typedef_declaration;
    friend class ast::enum_declaration;
    friend class ast::class_declaration;
    friend class ast::formula_statement;
#ifdef BUILD_LISTENERS
    friend class core_listener;
#endif

  public:
    RIDDLE_EXPORT core();
    virtual ~core() = default;

    /**
     * @brief Parses the given riddle script.
     *
     * @param script The riddle script to parse.
     */
    RIDDLE_EXPORT virtual void read(const std::string &script);
    /**
     * @brief Parses the given riddle files.
     *
     * @param files The riddle files to parse.
     */
    RIDDLE_EXPORT virtual void read(const std::vector<std::string> &files);

    core &get_core() override { return *this; }

    /**
     * @brief Creates a new boolean expression.
     *
     * @return expr The new boolean expression.
     */
    virtual expr new_bool() = 0;
    /**
     * @brief Creates a new boolean expression with the given value.
     *
     * @param value The value of the new boolean expression.
     * @return expr The new boolean expression.
     */
    virtual expr new_bool(bool value) = 0;

    /**
     * @brief Creates a new integer expression.
     *
     * @return expr The new integer expression.
     */
    virtual expr new_int() = 0;
    /**
     * @brief Creates a new integer expression with the given value.
     *
     * @param value The value of the new integer expression.
     * @return expr The new integer expression.
     */
    virtual expr new_int(utils::I value) = 0;

    /**
     * @brief Creates a new real expression.
     *
     * @return expr The new real expression.
     */
    virtual expr new_real() = 0;
    /**
     * @brief Creates a new real expression with the given value.
     *
     * @param value The value of the new real expression.
     * @return expr The new real expression.
     */
    virtual expr new_real(utils::rational value) = 0;

    /**
     * @brief Creates a new time point expression.
     *
     * @return expr The new time point expression.
     */
    virtual expr new_time_point() = 0;
    /**
     * @brief Creates a new time point expression with the given value.
     *
     * @param value The value of the new time point expression.
     * @return expr The new time point expression.
     */
    virtual expr new_time_point(utils::rational value) = 0;

    /**
     * @brief Creates a new string expression.
     *
     * @return expr The new string expression.
     */
    virtual expr new_string() = 0;
    /**
     * @brief Creates a new string expression with the given value.
     *
     * @param value The value of the new string expression.
     * @return expr The new string expression.
     */
    virtual expr new_string(const std::string &value) = 0;

    /**
     * @brief Creates a new object expression.
     *
     * @param tp The type of the object expression.
     * @return expr The new object expression.
     */
    virtual expr new_item(complex_type &tp) = 0;

    /**
     * @brief Creates a new enum expression.
     *
     * @param tp The type of the enum expression.
     * @param xprs The values of the enum expression.
     * @return expr The new enum expression.
     */
    virtual expr new_enum(type &tp, const std::vector<expr> &xprs) = 0;

    /**
     * @brief Returns the expression with the given name in the given scope.
     *
     * @param xpr The scope to search in.
     * @param name The name of the expression to search for.
     * @return expr The expression with the given name in the given scope.
     */
    virtual expr get_enum(expr &xpr, const std::string &name) = 0;

    /**
     * @brief Creates a new arithmetic expression representing the sum of the given expressions.
     *
     * @param xprs The expressions to sum.
     * @return expr A new arithmetic expression representing the sum of the given expressions.
     */
    virtual expr add(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new arithmetic expression representing the difference of the given expressions.
     *
     * @param xprs The expressions to subtract.
     * @return expr A new arithmetic expression representing the difference of the given expressions.
     */
    virtual expr sub(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new arithmetic expression representing the product of the given expressions.
     *
     * @param xprs The expressions to multiply.
     * @return expr A new arithmetic expression representing the product of the given expressions.
     */
    virtual expr mul(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new arithmetic expression representing the quotient of the given expressions.
     *
     * @param xprs The expressions to divide.
     * @return expr A new arithmetic expression representing the quotient of the given expressions.
     */
    virtual expr div(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new arithmetic expression representing the opposite of the given expression.
     *
     * @param xprs The expression to negate.
     * @return expr A new arithmetic expression representing the opposite of the given expression.
     */
    virtual expr minus(const expr &xprs) = 0;

    /**
     * @brief Creates a new propositional expression representing whether the given expressions are one strictly less than the other.
     *
     * @param lhs The left hand side of the comparison.
     * @param rhs The right hand side of the comparison.
     * @return expr A new propositional expression representing whether the given expressions are one strictly less than the other.
     */
    virtual expr lt(const expr &lhs, const expr &rhs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether the given expressions are one less than or equal to the other.
     *
     * @param lhs The left hand side of the comparison.
     * @param rhs The right hand side of the comparison.
     * @return expr A new propositional expression representing whether the given expressions are one less than or equal to the other.
     */
    virtual expr leq(const expr &lhs, const expr &rhs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether the given expressions are one strictly greater than the other.
     *
     * @param lhs The left hand side of the comparison.
     * @param rhs The right hand side of the comparison.
     * @return expr A new propositional expression representing whether the given expressions are one strictly greater than the other.
     */
    virtual expr gt(const expr &lhs, const expr &rhs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether the given expressions are one greater than or equal to the other.
     *
     * @param lhs The left hand side of the comparison.
     * @param rhs The right hand side of the comparison.
     * @return expr A new propositional expression representing whether the given expressions are one greater than or equal to the other.
     */
    virtual expr geq(const expr &lhs, const expr &rhs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether the given expressions are equal.
     *
     * @param lhs The left hand side of the comparison.
     * @param rhs The right hand side of the comparison.
     * @return expr A new propositional expression representing whether the given expressions are equal.
     */
    virtual expr eq(const expr &lhs, const expr &rhs) = 0;

    /**
     * @brief Creates a new propositional expression representing whether the conjunction of the given expressions is true.
     *
     * @param xprs The expressions to conjunct.
     * @return expr A new propositional expression representing whether the conjunction of the given expressions is true.
     */
    virtual expr conj(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether the disjunction of the given expressions is true.
     *
     * @param xprs The expressions to disjunct.
     * @return expr A new propositional expression representing whether the disjunction of the given expressions is true.
     */
    virtual expr disj(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new propositional expression representing whether exactly one of the given expressions is true.
     *
     * @param xprs The expressions to choose from.
     * @return expr A new propositional expression representing whether exactly one of the given expressions is true.
     */
    virtual expr exct_one(const std::vector<expr> &xprs) = 0;
    /**
     * @brief Creates a new propositional expression representing the negation of the given expression.
     *
     * @param xpr The expression to negate.
     * @return expr A new propositional expression representing the negation of the given expression.
     */
    virtual expr negate(const expr &xpr) = 0;

    /**
     * @brief Asserts the given expression as a fact.
     *
     * @param xpr The expression to assert.
     */
    virtual void assert_fact(const expr &xpr) = 0;

    /**
     * @brief Creates a new disjunction of the given conjunctions.
     *
     * @param conjs The conjunctions to disjunct.
     */
    virtual void new_disjunction(std::vector<conjunction_ptr> conjs) = 0;

    /**
     * @brief Creates a new expression representing a fact of the given predicate.
     *
     * @param pred The predicate of the fact.
     * @return expr A new expression representing a fact of the given predicate.
     */
    virtual expr new_fact(predicate &pred) = 0;
    /**
     * @brief Creates a new expression representing a goal of the given predicate.
     *
     * @param pred The predicate of the goal.
     * @return expr A new expression representing a goal of the given predicate.
     */
    virtual expr new_goal(predicate &pred) = 0;

  private:
    virtual void new_atom(expr &atm) = 0;

  public:
    /**
     * @brief Returns whether the given expression is a constant.
     *
     * @param xpr The expression to check.
     * @return true If the given expression is a constant.
     * @return false If the given expression is not a constant.
     */
    virtual bool is_constant(const expr &xpr) const = 0;
    /**
     * @brief Returns the boolean value of the given expression.
     *
     * @param xpr The expression to evaluate.
     * @return utils::lbool The boolean value of the given expression.
     */
    virtual utils::lbool bool_value(const expr &xpr) const = 0;
    /**
     * @brief Returns the infinitesimal rational value of the given expression.
     *
     * @param xpr The expression to evaluate.
     * @return utils::inf_rational The infinitesimal rational value of the given expression.
     */
    virtual utils::inf_rational arith_value(const expr &xpr) const = 0;
    /**
     * @brief Returns the bounds of the given expression.
     *
     * @param xpr The expression to evaluate.
     * @return std::pair<utils::inf_rational, utils::inf_rational> The bounds of the given expression.
     */
    virtual std::pair<utils::inf_rational, utils::inf_rational> arith_bounds(const expr &xpr) const = 0;
    /**
     * @brief Returns the time value of the given expression.
     *
     * @param xpr The expression to evaluate.
     * @return utils::inf_rational The time value of the given expression.
     */
    virtual utils::inf_rational time_value(const expr &xpr) const = 0;
    /**
     * @brief Returns the bounds of the given expression.
     *
     * @param xpr The expression to evaluate.
     * @return std::pair<utils::inf_rational, utils::inf_rational> The bounds of the given expression.
     */
    virtual std::pair<utils::inf_rational, utils::inf_rational> time_bounds(const expr &xpr) const = 0;

    /**
     * @brief Returns whether the given expression is an enumeration.
     *
     * @param xpr The expression to check.
     * @return true If the given expression is an enumeration.
     * @return false If the given expression is not an enumeration.
     */
    virtual bool is_enum(const expr &xpr) const = 0;
    /**
     * @brief Returns the domain of the given expression. If the expression is not an enumeration, the domain is empty.
     *
     * @param xpr The expression to get the domain of.
     * @return std::vector<expr> The domain of the given expression.
     */
    virtual std::vector<expr> domain(const expr &xpr) const = 0;
    /**
     * @brief Removes the given value from the domain of the given expression.
     *
     * @param xpr The expression to prune.
     * @param val The value to remove from the domain of the given expression.
     */
    virtual void prune(const expr &xpr, const expr &val) = 0;

    /**
     * @brief Get the bool type object.
     *
     * @return type& The bool type.
     */
    type &get_bool_type() const noexcept { return *b_tp; }
    /**
     * @brief Get the int type object.
     *
     * @return type& The int type.
     */
    type &get_int_type() const noexcept { return *i_tp; }
    /**
     * @brief Get the real type object.
     *
     * @return type& The real type.
     */
    type &get_real_type() const noexcept { return *r_tp; }
    /**
     * @brief Get the time type object.
     *
     * @return type& The time type.
     */
    type &get_time_type() const noexcept { return *t_tp; }
    /**
     * @brief Get the string type object.
     *
     * @return type& The string type.
     */
    type &get_string_type() const noexcept { return *s_tp; }

    RIDDLE_EXPORT field &get_field(const std::string &name) const override;

    bool has_type(const std::string &nm) const override { return types.find(nm) != types.end(); }

    /**
     * @brief Get the type of the given expressions. If the expressions are all integers, the integer type is returned. If the expressions are all reals, the real type is returned. If the expressions are all times or constants, the time type is returned. Otherwise, the real type is returned.
     *
     * @param args The expressions to get the type of.
     * @return type& The type of the given expressions.
     */
    RIDDLE_EXPORT type &get_type(const std::vector<expr> &args);

    /**
     * @brief Returns the type with the given name.
     *
     * @param name The name of the type to get.
     * @return type& The type with the given name.
     */
    RIDDLE_EXPORT type &get_type(const std::string &name) const override;
    /**
     * @brief Returns all the types in this core.
     *
     * @return std::vector<std::reference_wrapper<type>> All types in this core.
     */
    RIDDLE_EXPORT std::vector<std::reference_wrapper<type>> get_types() const override;
    /**
     * @brief Returns the method with the given name and argument types.
     *
     * @param name The name of the method to get.
     * @param args The argument types of the method to get.
     * @return method& The method with the given name and argument types.
     */
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) const override;
    /**
     * @brief Returns the predicate with the given name.
     *
     * @param name The name of the predicate to get.
     * @return predicate& The predicate with the given name.
     */
    RIDDLE_EXPORT predicate &get_predicate(const std::string &name) const override;

    /**
     * @brief Returns all the predicates in this core.
     *
     * @return std::vector<std::reference_wrapper<predicate>> All predicates in this core.
     */
    RIDDLE_EXPORT std::vector<std::reference_wrapper<predicate>> get_predicates() const override;

  protected:
    /**
     * @brief Adds the given type to this core.
     *
     * @param tp The type to add.
     */
    void add_type(type_ptr tp) { types.emplace(tp->get_name(), std::move(tp)); }
    /**
     * @brief Adds the given method to this core.
     *
     * @param mthd The method to add.
     */
    void add_method(method_ptr mthd) { methods[mthd->get_name()].emplace_back(std::move(mthd)); }
    /**
     * @brief Adds the given predicate to this core.
     *
     * @param pred The predicate to add.
     */
    void add_predicate(predicate_ptr pred) { predicates.emplace(pred->get_name(), std::move(pred)); }

    /**
     * @brief Adds the given expression to the given predicate.
     *
     * @param xpr The expression to add.
     */
    void add_atom(predicate &pred, expr atm) { pred.instances.emplace_back(atm); }

#ifdef COMPUTE_NAMES
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
    type *b_tp, *i_tp, *r_tp, *t_tp, *s_tp;     // builtin types..
    std::vector<ast::compilation_unit_ptr> cus; // the compilation units..

  private:
    std::map<std::string, type_ptr> types;
    std::map<std::string, std::vector<method_ptr>> methods;
    std::map<std::string, predicate_ptr> predicates;

#ifdef BUILD_LISTENERS
  private:
    std::vector<core_listener *> listeners; // the core listeners..

  protected:
    RIDDLE_EXPORT void fire_log(const std::string &msg) const noexcept;
    RIDDLE_EXPORT void fire_read(const std::string &script) const noexcept;
    RIDDLE_EXPORT void fire_read(const std::vector<std::string> &files) const noexcept;
    RIDDLE_EXPORT void fire_state_changed() const noexcept;
    RIDDLE_EXPORT void fire_started_solving() const noexcept;
    RIDDLE_EXPORT void fire_solution_found() const noexcept;
    RIDDLE_EXPORT void fire_inconsistent_problem() const noexcept;
#endif
  };

  inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }

  class unsolvable_exception : public std::exception
  {
    const char *what() const noexcept override { return "the problem is unsolvable.."; }
  };
} // namespace riddle