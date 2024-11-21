#pragma once

#include "expression.hpp"

namespace riddle
{
  /**
   * @class core core.hpp "include/core.hpp"
   * @brief The core environment class.
   */
  class core : public env
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
    [[nodiscard]] std::shared_ptr<bool_item> new_bool(bool value);

    [[nodiscard]] std::shared_ptr<item> get(std::string_view name) noexcept override;
  };
} // namespace riddle
