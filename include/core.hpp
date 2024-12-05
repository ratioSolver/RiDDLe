#pragma once

#include "scope.hpp"
#include "expression.hpp"

namespace riddle
{
  /**
   * @class core core.hpp "include/core.hpp"
   * @brief The core environment class.
   */
  class core : public scope, public env
  {
  public:
    core() noexcept;
    virtual ~core() = default;

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
    [[nodiscard]] std::shared_ptr<arith_item> new_real(const utils::rational &value);

    [[nodiscard]] field &get_field(std::string_view name) const override;

    [[nodiscard]] method &get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;
    [[nodiscard]] type &get_type(std::string_view name) const override;
    [[nodiscard]] predicate &get_predicate(std::string_view name) const override;

    [[nodiscard]] item &get(std::string_view name) override;

  private:
    std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> methods; // the methods declared in the core..
    std::map<std::string, std::unique_ptr<type>, std::less<>> types;                  // the types declared in the core..
    std::map<std::string, std::unique_ptr<predicate>, std::less<>> predicates;        // the predicates declared in the core..
  };
} // namespace riddle
