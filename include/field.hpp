#pragma once

#include "expression.hpp"
#include <string>

namespace riddle
{
  class init_element;
  class type;

  class field
  {
  public:
    field(type &t, std::string_view name, const std::vector<std::unique_ptr<expression>> &inits = {}, bool synthetic = false) : t(t), name(name), inits(inits), synthetic(synthetic) {}

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
     * @brief Get the initializations.
     *
     * @return The initializations.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<expression>> &get_inits() const noexcept { return inits; }
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
    const std::vector<std::unique_ptr<expression>> &inits;
    bool synthetic;
  };
} // namespace riddle
