#pragma once

#include "bool.hpp"
#include "inf_rational.hpp"
#include "type.hpp"
#include "parser.hpp"

#ifdef COMPUTE_NAMES
#include <unordered_map>

#define RECOMPUTE_NAMES() recompute_names()
#else
#define RECOMPUTE_NAMES()
#endif

namespace riddle
{
  class conjunction;

  /**
   * @class core core.hpp "include/core.hpp"
   * @brief The core environment class.
   */
  class core : public scope, public env
  {
    friend class enum_declaration;
    friend class method_declaration;
    friend class class_declaration;
    friend class predicate_declaration;
#ifdef COMPUTE_NAMES
    friend class component;
    friend class component_type;
#endif

  public:
    core(std::string_view name = "RiDDLe") noexcept;

    /**
     * @brief Retrieves the unique identifier for the current core.
     *
     * This function returns the unique identifier for the current core.
     *
     * @return uintptr_t The unique identifier.
     */
    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    /**
     * @brief Retrieves the name of the core environment.
     *
     * This function returns the name as a string.
     *
     * @return std::string The name.
     */
    [[nodiscard]] std::string get_name() const noexcept { return name; }

    /**
     * @brief Reads and processes the given RiDDLe script.
     *
     * This function reads and processes the given RiDDLe script.
     *
     * @param script The RiDDLe script.
     */
    virtual void read(std::string_view script);
    /**
     * @brief Reads and processes the given list of RiDDLe files.
     *
     * This function reads and processes the given list of RiDDLe files.
     *
     * @param files The list of RiDDLe files.
     */
    virtual void read(const std::vector<std::string> &files);

    /**
     * @brief Create a new bool expression.
     *
     * @return bool_expr The bool expression.
     */
    [[nodiscard]] virtual bool_expr new_bool() = 0;
    /**
     * @brief Create a new bool expression with a value.
     *
     * @param value The value of the bool expression.
     * @return bool_expr The bool expression.
     */
    [[nodiscard]] virtual bool_expr new_bool(const bool value) = 0;

    /**
     * @brief Evaluates the boolean value of the given expression.
     *
     * This function takes a boolean expression and returns its logical value.
     *
     * @param expr The boolean expression to evaluate.
     * @return utils::lbool The logical value of the expression.
     */
    [[nodiscard]] virtual utils::lbool bool_value(const bool_term &expr) const noexcept = 0;

    /**
     * @brief Create a new int expression.
     *
     * @return arith_expr The int expression.
     */
    [[nodiscard]] virtual arith_expr new_int() = 0;
    /**
     * @brief Create a new int expression with a value.
     *
     * @param value The value of the int expression.
     * @return arith_expr The int expression.
     */
    [[nodiscard]] virtual arith_expr new_int(const INT_TYPE value) = 0;
    /**
     * @brief Create a new int expression with the given bounds.
     *
     * @param lb The lower bound of the int expression.
     * @param ub The upper bound of the int expression.
     * @return arith_expr The int expression.
     */
    [[nodiscard]] virtual arith_expr new_int(const INT_TYPE lb, const INT_TYPE ub) = 0;
    /**
     * @brief Create a new uncertain int expression with the given bounds.
     *
     * @param lb The lower bound of the uncertain int expression.
     * @param ub The upper bound of the uncertain int expression.
     * @return arith_expr The uncertain int expression.
     */
    [[nodiscard]] virtual arith_expr new_uncertain_int(const INT_TYPE lb, const INT_TYPE ub) = 0;

    /**
     * @brief Create a real expression.
     *
     * @return arith_expr The real expression.
     */
    [[nodiscard]] virtual arith_expr new_real() = 0;
    /**
     * @brief Create a new real expression with a value.
     *
     * @param value The value of the real expression.
     * @return arith_expr The real expression.
     */
    [[nodiscard]] virtual arith_expr new_real(utils::rational &&value) = 0;
    /**
     * @brief Create a new real expression with the given bounds.
     *
     * @param lb The lower bound of the real expression.
     * @param ub The upper bound of the real expression.
     * @return arith_expr The real expression.
     */
    [[nodiscard]] virtual arith_expr new_real(utils::rational &&lb, utils::rational &&ub) = 0;
    /**
     * @brief Create a new uncertain real expression with the given bounds.
     *
     * @param lb The lower bound of the uncertain real expression.
     * @param ub The upper bound of the uncertain real expression.
     * @return arith_expr The uncertain real expression.
     */
    [[nodiscard]] virtual arith_expr new_uncertain_real(utils::rational &&lb, utils::rational &&ub) = 0;

    /**
     * @brief Create a new time expression.
     *
     * @return arith_expr The time expression.
     */
    [[nodiscard]] virtual arith_expr new_time() = 0;
    /**
     * @brief Create a new time expression with a value.
     *
     * @param value The value of the time expression.
     * @return arith_expr The time expression.
     */
    [[nodiscard]] virtual arith_expr new_time(utils::rational &&value) = 0;

    /**
     * @brief Computes the arithmetic value of the given arithmetic item.
     *
     * This function takes an arithmetic item expression and computes its
     * corresponding arithmetic value.
     *
     * @param expr The arithmetic item expression to be evaluated.
     * @return utils::inf_rational The computed arithmetic value of the expression.
     */
    [[nodiscard]] virtual utils::inf_rational arith_value(const arith_term &expr) const noexcept = 0;

    /**
     * @brief Create a new string expression.
     *
     * @return string_expr The string expression.
     */
    [[nodiscard]] virtual string_expr new_string() = 0;
    /**
     * @brief Create a new string expression with a value.
     *
     * @param value The value of the string expression.
     * @return string_expr The string expression.
     */
    [[nodiscard]] virtual string_expr new_string(std::string &&value) = 0;
    /**
     * @brief Retrieves the string value of the given expression.
     *
     * This function takes a string expression and returns its string value.
     *
     * @param expr The string expression.
     * @return std::string The string value of the expression.
     */
    [[nodiscard]] virtual std::string string_value(const string_term &expr) const noexcept = 0;

    /**
     * @brief Creates a new enum item.
     *
     * This function is responsible for creating a new enum item based on the provided type and values.
     *
     * @param tp A reference to the type for which the enum item is being created.
     * @param values A vector of shared pointers to the values that make up the enum item.
     *
     * @return expr A shared pointer to the newly created enum item.
     */
    [[nodiscard]] virtual expr new_enum(component_type &tp, std::vector<expr> &&values) = 0;
    /**
     * @brief Retrieves the domain for the given enum item.
     *
     * This function takes an enum item and returns its associated domain as a vector of expressions.
     *
     * @param expr The enum item for which the domain is being retrieved.
     * @return std::vector<expr> A vector of expressions representing the domain of the enum item.
     */
    [[nodiscard]] virtual std::vector<expr> enum_value(const enum_term &expr) const noexcept = 0;

    /**
     * @brief Creates a new boolean item representing a logical AND operation.
     *
     * This function takes a vector of boolean items and combines them into a single
     * boolean item that represents the logical AND of all the input expressions.
     *
     * @param exprs A vector of shared pointers to boolean items to be combined.
     * @return A shared pointer to the newly created boolean item representing the AND operation.
     */
    [[nodiscard]] bool_expr new_and(std::vector<bool_expr> &&exprs);
    /**
     * @brief Creates a new boolean OR item from a list of boolean expressions.
     *
     * This function is responsible for creating a new boolean OR item, which
     * represents the logical OR operation on the provided boolean expressions.
     *
     * @param exprs A vector of shared pointers to boolean items, representing
     * the boolean expressions to be OR-ed together.
     * @return A shared pointer to the newly created boolean OR item.
     */
    [[nodiscard]] bool_expr new_or(std::vector<bool_expr> &&exprs);
    /**
     * @brief Creates a new XOR (exclusive OR) boolean item.
     *
     * This function is responsible for creating a new XOR boolean item from a vector of boolean items.
     * The XOR operation will return true if an odd number of the boolean items in the vector are true.
     *
     * @param exprs A vector of shared pointers to boolean items, which will be used as the operands for the XOR operation.
     * @return A shared pointer to the newly created XOR boolean item.
     */
    [[nodiscard]] bool_expr new_xor(std::vector<bool_expr> &&exprs);

    /**
     * @brief Creates a new boolean item representing the logical NOT of the given expression.
     *
     * This function takes a shared pointer to a boolean item and returns a new shared pointer
     * to a boolean item that represents the logical NOT operation applied to the input expression.
     *
     * @param expr A shared pointer to the boolean item to be negated.
     * @return A shared pointer to a new boolean item representing the logical NOT of the input expression.
     */
    [[nodiscard]] bool_expr new_not(bool_expr expr);

    /**
     * @brief Creates a new arithmetic item representing the negation of the given expression.
     *
     * This function takes a shared pointer to an arithmetic item and returns a shared pointer
     * to a new arithmetic item that represents the negation of the input expression.
     *
     * @param xpr A shared pointer to the arithmetic item to be negated.
     * @return A shared pointer to the new arithmetic item representing the negation of the input expression.
     */
    [[nodiscard]] virtual arith_expr new_negation(arith_expr xpr) = 0;

    /**
     * @brief Creates a new arithmetic sum item.
     *
     * This function is responsible for creating a new arithmetic sum item
     * from a vector of arithmetic items. The function returns a shared pointer
     * to the newly created arithmetic sum item.
     *
     * @param xprs A vector of shared pointers to arithmetic items that will be summed.
     * @return arith_expr A shared pointer to the newly created arithmetic sum item.
     */
    [[nodiscard]] virtual arith_expr new_sum(std::vector<arith_expr> &&xprs) = 0;
    /**
     * @brief Creates a new arithmetic subtraction item.
     *
     * This function is responsible for creating a new arithmetic subtraction item
     * from a vector of arithmetic items. The function returns a shared pointer
     * to the newly created arithmetic subtraction item.
     *
     * @param xprs A vector of shared pointers to arithmetic items that will be subtracted.
     * @return arith_expr A shared pointer to the newly created arithmetic subtraction item.
     */
    [[nodiscard]] virtual arith_expr new_subtraction(std::vector<arith_expr> &&xprs) = 0;
    /**
     * @brief Creates a new product from a vector of arithmetic items.
     *
     * This function is responsible for creating a new product object
     * from the given vector of arithmetic items. The function returns
     * a shared pointer to the newly created product.
     *
     * @param xprs A vector of shared pointers to arithmetic items.
     *
     * @return arith_expr A shared pointer to the newly created product.
     */
    [[nodiscard]] virtual arith_expr new_product(std::vector<arith_expr> &&xprs) = 0;
    /**
     * @brief Creates a new division arithmetic item.
     *
     * This function is responsible for creating a new division arithmetic item
     * from a vector of arithmetic items. The function returns a shared pointer
     * to the newly created division arithmetic item.
     *
     * @param xprs A vector of shared pointers to arithmetic items that will be divided.
     *
     * @return arith_expr A shared pointer to the newly created division arithmetic item.
     */
    [[nodiscard]] virtual arith_expr new_division(std::vector<arith_expr> &&xprs) = 0;

    /**
     * @brief Creates a new less-than comparison item.
     *
     * This function creates a new less-than comparison item using the provided
     * arithmetic items as the left-hand side (lhs) and right-hand side (rhs) of
     * the comparison.
     *
     * @param lhs A shared pointer to the left-hand side arithmetic item.
     * @param rhs A shared pointer to the right-hand side arithmetic item.
     * @return A shared pointer to the newly created less-than comparison item.
     */
    [[nodiscard]] bool_expr new_lt(arith_expr lhs, arith_expr rhs);
    /**
     * @brief Creates a new less-than-or-equal-to (<=) boolean item.
     *
     * This function generates a new boolean item that represents the
     * less-than-or-equal-to (<=) comparison between two arithmetic items.
     *
     * @param lhs A shared pointer to the left-hand side arithmetic item.
     * @param rhs A shared pointer to the right-hand side arithmetic item.
     * @return A shared pointer to the newly created boolean item representing
     *         the result of the <= comparison.
     */
    [[nodiscard]] bool_expr new_le(arith_expr lhs, arith_expr rhs);
    /**
     * @brief Creates a new greater-than comparison item.
     *
     * This function creates a new boolean item that represents the result of
     * comparing two arithmetic items to determine if the left-hand side (lhs)
     * is greater than the right-hand side (rhs).
     *
     * @param lhs A shared pointer to the left-hand side arithmetic item.
     * @param rhs A shared pointer to the right-hand side arithmetic item.
     * @return A shared pointer to a boolean item representing the result of the comparison.
     */
    [[nodiscard]] bool_expr new_gt(arith_expr lhs, arith_expr rhs);
    /**
     * @brief Creates a new greater-than-or-equal-to boolean item.
     *
     * This function creates a new boolean item that represents the
     * greater-than-or-equal-to (>=) comparison between two arithmetic items.
     *
     * @param lhs A shared pointer to the left-hand side arithmetic item.
     * @param rhs A shared pointer to the right-hand side arithmetic item.
     * @return A shared pointer to the newly created boolean item representing the comparison.
     */
    [[nodiscard]] bool_expr new_ge(arith_expr lhs, arith_expr rhs);
    /**
     * @brief Creates a new equality comparison item.
     *
     * This function creates a new equality comparison item using the provided left-hand side (lhs)
     * and right-hand side (rhs) items.
     *
     * @param lhs A shared pointer to the left-hand side item.
     * @param rhs A shared pointer to the right-hand side item.
     * @return A shared pointer to the newly created equality comparison item.
     */
    [[nodiscard]] bool_expr new_eq(expr lhs, expr rhs);

    /**
     * @brief Asserts a clause composed of a vector of boolean expressions.
     *
     * This pure virtual function must be implemented by derived classes to handle
     * the assertion of a clause, which is represented as a vector of boolean expressions.
     *
     * @param exprs A vector of boolean expressions (bool_expr) to be asserted.
     */
    virtual void new_clause(std::vector<bool_expr> &&exprs) = 0;

    /**
     * @brief Pure virtual function to add a new disjunction.
     *
     * This function is responsible for adding a new disjunction composed of multiple conjunctions.
     *
     * @param disjuncts A vector of unique pointers to conjunction objects, representing the disjunction.
     */
    virtual void new_disjunction(std::vector<std::unique_ptr<conjunction>> &&disjuncts) = 0;

    /**
     * @brief Creates a new atom.
     *
     * @param is_fact A boolean indicating whether the atom is a fact or a goal.
     * @param pred A reference to the predicate associated with the atom.
     * @param args An optional map of arguments where the key is a string and the value is a shared pointer to an item. Defaults to an empty map.
     * @return A shared pointer to the newly created atom.
     */
    [[nodiscard]] atom_expr new_atom(bool is_fact, predicate &pred, std::map<std::string, expr, std::less<>> &&args = {});
    [[nodiscard]] virtual atom_state get_atom_state(const atom_term &atom) const noexcept = 0;

    [[nodiscard]] field &get_field(std::string_view name) const override;

    [[nodiscard]] const std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> &get_methods() const { return methods; }
    [[nodiscard]] method &get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;

    [[nodiscard]] const std::map<std::string, std::unique_ptr<type>, std::less<>> &get_types() const noexcept override { return types; }
    [[nodiscard]] type &get_type(std::string_view name) const override;

    [[nodiscard]] const std::map<std::string, std::unique_ptr<predicate>, std::less<>> &get_predicates() const { return predicates; }
    [[nodiscard]] predicate &get_predicate(std::string_view name) const override;

    /**
     * @brief Promotes the type of the given arithmetic expressions.
     *
     * This function takes a vector of arithmetic expressions and determines the
     * appropriate type promotion for them. The result is a reference to the
     * promoted type.
     *
     * @param exprs A vector of arithmetic expressions to be promoted.
     * @return A reference to the promoted type.
     */
    [[nodiscard]] type &type_promotion(const std::vector<arith_expr> &exprs) const;

    [[nodiscard]] expr get(std::string_view name) override;

    [[nodiscard]] virtual json::json to_json() const override;

  protected:
    /**
     * @brief Adds a method to this RiDDLe core.
     *
     * This function takes ownership of the provided unique pointer to a method
     * and adds it to the internal collection of methods.
     *
     * @param mthd A unique pointer to the method to be added.
     */
    void add_method(std::unique_ptr<method> mthd);

    /**
     * @brief Adds a predicate to this RiDDLe core.
     *
     * This function takes ownership of the provided predicate and adds it to the
     * internal collection of predicates.
     *
     * @param pred A unique pointer to the predicate to be added.
     */
    void add_predicate(std::unique_ptr<predicate> pred);

    /**
     * @brief Adds a type to this RiDDLe core.
     *
     * This function takes ownership of the provided type and adds it to the
     * internal collection of types.
     *
     * @param tp A unique pointer to the type to be added.
     */
    void add_type(std::unique_ptr<type> tp);

#ifdef COMPUTE_NAMES
  protected:
    /**
     * @brief Guesses the name of the given term.
     *
     * This function attempts to find the name associated with the provided term
     * by looking it up in the `expr_names` map. If the term is found, its
     * corresponding name is returned. Otherwise, an empty string is returned.
     *
     * @param itm The term for which the name is to be guessed.
     * @return A string representing the name of the term, or an empty string if
     *         the term is not found in the `expr_names` map.
     */
    std::string guess_name(const term &itm) const noexcept
    {
      if (const auto at_f = expr_names.find(&itm); at_f != expr_names.cend())
        return at_f->second;
      return "";
    }

  private:
    /**
     * @brief Recomputes the names.
     *
     * This function recalculates or updates the names based on the current state.
     * It is guaranteed not to throw any exceptions.
     */
    void recompute_names() noexcept;
#endif

  private:
    [[nodiscard]] virtual atom_expr create_atom(bool is_fact, predicate &pred, std::map<std::string, expr, std::less<>> &&args = {}) = 0;

  private:
    const std::string name;                                                           // the name of the core..
    std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> methods; // the methods declared in the core..
    std::map<std::string, std::unique_ptr<type>, std::less<>> types;                  // the types declared in the core..
    std::map<std::string, std::unique_ptr<predicate>, std::less<>> predicates;        // the predicates declared in the core..
    std::vector<std::unique_ptr<compilation_unit>> cus;                               // the compilation units read by the core..

#ifdef COMPUTE_NAMES
    std::unordered_map<const term *, const std::string> expr_names; // the names of the expressions..
#endif
  };

  [[nodiscard]] inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }

  [[nodiscard]] json::json to_json(const utils::rational &r) noexcept;
  [[nodiscard]] json::json to_json(const utils::inf_rational &r) noexcept;
} // namespace riddle
