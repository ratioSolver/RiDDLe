#pragma once

#include <string>
#include "expression.hpp"

namespace riddle
{
  class type;

  class field
  {
  public:
    field(type &t, const std::string &name, std::unique_ptr<expression> &&init = nullptr, bool synthetic = false) : t(t), name(name), init(std::move(init)), synthetic(synthetic) {}

    /**
     * @brief Get the type.
     *
     * @return The type.
     */
    [[nodiscard]] type &get_type() const noexcept { return t; }
    /**
     * @brief Get the name.
     *
     * @return The name.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }
    /**
     * @brief Get the initializer.
     *
     * @return The initializer.
     */
    [[nodiscard]] const std::unique_ptr<expression> &get_init() const noexcept { return init; }
    /**
     * @brief Check if this field is synthetic.
     *
     * @return true If this field is synthetic.
     * @return false If this field is not synthetic.
     */
    [[nodiscard]] bool is_synthetic() const noexcept { return synthetic; }

  private:
    type &t;
    std::string name;
    std::unique_ptr<expression> init;
    bool synthetic;
  };
} // namespace riddle
