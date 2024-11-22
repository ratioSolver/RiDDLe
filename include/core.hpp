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
    [[nodiscard]] std::shared_ptr<bool_item> new_bool(bool value);

    [[nodiscard]] field &get_field(std::string_view name) const override;

    [[nodiscard]] type &get_type(std::string_view name) const override;

    [[nodiscard]] item &get(std::string_view name) override;

  private:
    std::map<std::string, std::unique_ptr<type>, std::less<>> types; // the types..
  };
} // namespace riddle
