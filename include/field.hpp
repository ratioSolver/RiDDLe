#pragma once

#include <string>

namespace riddle
{
  class type;

  class field
  {
  public:
    field(type &t, const std::string &name) : t(t), name(name) {}

    /**
     * @brief Get the type.
     *
     * @return The type.
     */
    type &get_type() const noexcept { return t; }
    /**
     * @brief Get the name.
     *
     * @return The name.
     */
    const std::string &get_name() const noexcept { return name; }

  private:
    type &t;
    std::string name;
  };
} // namespace riddle
