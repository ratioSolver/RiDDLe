#pragma once

#include <string>
#include "expression.hpp"

namespace riddle
{
  class init_element;
  class type;

  class field
  {
  public:
    field(type &t, const std::string &name, const std::vector<init_element> &inits = {}, bool synthetic = false) : t(t), name(name), inits(inits), synthetic(synthetic) {}

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
    [[nodiscard]] const std::vector<init_element> &get_inits() const noexcept { return inits; }
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
    const std::vector<init_element> &inits;
    bool synthetic;
  };
} // namespace riddle
