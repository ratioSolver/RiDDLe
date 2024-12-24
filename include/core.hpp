#pragma once

#include "type.hpp"

namespace riddle
{
  class enum_declaration;
  class method_declaration;
  class class_declaration;
  class predicate_declaration;
  class compilation_unit;

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

  public:
    core() noexcept;
    virtual ~core() = default;

    /**
     * @brief Reads and processes the given RiDDLe script.
     *
     * This function reads and processes the given RiDDLe script.
     *
     * @param script The RiDDLe script.
     */
    virtual void read(std::string &&script);
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
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_bool() = 0;
    /**
     * @brief Create a new bool expression with a value.
     *
     * @param value The value of the bool expression.
     * @return std::shared_ptr<bool_item> The bool expression.
     */
    [[nodiscard]] std::shared_ptr<bool_item> new_bool(const bool value);

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
    [[nodiscard]] std::shared_ptr<arith_item> new_int(const INT_TYPE value);
    /**
     * @brief Create a new int expression with the given bounds.
     *
     * @param lb The lower bound of the int expression.
     * @param ub The upper bound of the int expression.
     * @return std::shared_ptr<arith_item> The int expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_int(const INT_TYPE lb, const INT_TYPE ub) = 0;
    /**
     * @brief Create a new uncertain int expression with the given bounds.
     *
     * @param lb The lower bound of the uncertain int expression.
     * @param ub The upper bound of the uncertain int expression.
     * @return std::shared_ptr<arith_item> The uncertain int expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_uncertain_int(const INT_TYPE lb, const INT_TYPE ub) = 0;

    /**
     * @brief Create a real expression.
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
    [[nodiscard]] std::shared_ptr<arith_item> new_real(utils::rational &&value);
    /**
     * @brief Create a new real expression with the given bounds.
     *
     * @param lb The lower bound of the real expression.
     * @param ub The upper bound of the real expression.
     * @return std::shared_ptr<arith_item> The real expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_real(utils::rational &&lb, utils::rational &&ub) = 0;
    /**
     * @brief Create a new uncertain real expression with the given bounds.
     *
     * @param lb The lower bound of the uncertain real expression.
     * @param ub The upper bound of the uncertain real expression.
     * @return std::shared_ptr<arith_item> The uncertain real expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_uncertain_real(utils::rational &&lb, utils::rational &&ub) = 0;

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
    [[nodiscard]] std::shared_ptr<arith_item> new_time(utils::rational &&value);

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
    [[nodiscard]] std::shared_ptr<string_item> new_string(std::string &&value);

    /**
     * @brief Creates a new enum item.
     *
     * This function is responsible for creating a new enum item based on the provided type and values.
     *
     * @param tp A reference to the type for which the enum item is being created.
     * @param values A vector of references to enum values that will be used to create the enum item.
     *               The vector is passed as an rvalue reference to allow for efficient move semantics.
     *
     * @return A shared pointer to the newly created enum item.
     */
    [[nodiscard]] virtual std::shared_ptr<enum_item> new_enum(type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) = 0;

    /**
     * @brief Creates a new boolean item representing a logical AND operation.
     *
     * This function takes a vector of boolean items and combines them into a single
     * boolean item that represents the logical AND of all the input expressions.
     *
     * @param exprs A vector of shared pointers to boolean items to be combined.
     * @return A shared pointer to the newly created boolean item representing the AND operation.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_and(std::vector<std::shared_ptr<bool_item>> &&exprs) = 0;
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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_or(std::vector<std::shared_ptr<bool_item>> &&exprs) = 0;
    /**
     * @brief Creates a new XOR (exclusive OR) boolean item.
     *
     * This function is responsible for creating a new XOR boolean item from a vector of boolean items.
     * The XOR operation will return true if an odd number of the boolean items in the vector are true.
     *
     * @param exprs A vector of shared pointers to boolean items, which will be used as the operands for the XOR operation.
     * @return A shared pointer to the newly created XOR boolean item.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_xor(std::vector<std::shared_ptr<bool_item>> &&exprs) = 0;

    /**
     * @brief Creates a new boolean item representing the logical NOT of the given expression.
     *
     * This function takes a shared pointer to a boolean item and returns a new shared pointer
     * to a boolean item that represents the logical NOT operation applied to the input expression.
     *
     * @param expr A shared pointer to the boolean item to be negated.
     * @return A shared pointer to a new boolean item representing the logical NOT of the input expression.
     */
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_not(std::shared_ptr<bool_item> expr) = 0;

    /**
     * @brief Creates a new arithmetic item representing the negation of the given expression.
     *
     * This function takes a shared pointer to an arithmetic item and returns a shared pointer
     * to a new arithmetic item that represents the negation of the input expression.
     *
     * @param xpr A shared pointer to the arithmetic item to be negated.
     * @return A shared pointer to the new arithmetic item representing the negation of the input expression.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_negation(std::shared_ptr<arith_item> xpr) = 0;

    /**
     * @brief Creates a new arithmetic sum item.
     *
     * This function is responsible for creating a new arithmetic sum item
     * from a vector of arithmetic items. The function returns a shared pointer
     * to the newly created arithmetic sum item.
     *
     * @param xprs A vector of shared pointers to arithmetic items that will be summed.
     * @return std::shared_ptr<arith_item> A shared pointer to the newly created arithmetic sum item.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_sum(std::vector<std::shared_ptr<arith_item>> &&xprs) = 0;
    /**
     * @brief Creates a new product from a vector of arithmetic items.
     *
     * This function is responsible for creating a new product object
     * from the given vector of arithmetic items. The function returns
     * a shared pointer to the newly created product.
     *
     * @param xprs A vector of shared pointers to arithmetic items.
     *
     * @return std::shared_ptr<arith_item> A shared pointer to the newly created product.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_product(std::vector<std::shared_ptr<arith_item>> &&xprs) = 0;
    /**
     * @brief Creates a new division arithmetic item.
     *
     * This function creates a new arithmetic item representing the division of
     * the left-hand side (lhs) by the right-hand side (rhs).
     *
     * @param lhs A shared pointer to the left-hand side arithmetic item.
     * @param rhs A shared pointer to the right-hand side arithmetic item.
     * @return std::shared_ptr<arith_item> A shared pointer to the resulting division arithmetic item.
     */
    [[nodiscard]] virtual std::shared_ptr<arith_item> new_divide(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) = 0;

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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_lt(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) = 0;
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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_le(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) = 0;
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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_gt(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) = 0;
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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_ge(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) = 0;
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
    [[nodiscard]] virtual std::shared_ptr<bool_item> new_eq(std::shared_ptr<item> lhs, std::shared_ptr<item> rhs) = 0;

    [[nodiscard]] field &get_field(std::string_view name) const override;

    [[nodiscard]] method &get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;
    [[nodiscard]] type &get_type(std::string_view name) const override;
    [[nodiscard]] predicate &get_predicate(std::string_view name) const override;

    [[nodiscard]] std::shared_ptr<item> get(std::string_view name) override;

  protected:
    /**
     * @brief Adds a type to this RiDDLe core.
     *
     * This function adds a type to the collection of types.
     *
     * @param tp A unique pointer to the type to be added.
     */
    void add_type(std::unique_ptr<type> tp);

  private:
    std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> methods; // the methods declared in the core..
    std::map<std::string, std::unique_ptr<type>, std::less<>> types;                  // the types declared in the core..
    std::map<std::string, std::unique_ptr<predicate>, std::less<>> predicates;        // the predicates declared in the core..
    std::vector<std::unique_ptr<compilation_unit>> cus;                               // the compilation units read by the core..
  };

  inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }

  class unsolvable_exception : public std::exception
  {
    const char *what() const noexcept override { return "the problem is unsolvable.."; }
  };
} // namespace riddle
