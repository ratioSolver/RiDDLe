#pragma once

#include <string>

namespace riddle
{
  /**
   * @class type type.hpp "include/type.hpp"
   * @brief The type class.
   *
   * The type class is used to represent the type of an item. Each item has a type
   * that determines the kind of data it can store. Types can be primitive or
   * user-defined.
   */
  class type
  {
  public:
    type(std::string &&name, bool primitive = false) noexcept;
    virtual ~type() = default;

    /**
     * @brief Retrieves the name of the type.
     *
     * This function returns a constant reference to the name of the type.
     *
     * @return const std::string& A constant reference to the name of the type.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }

  private:
    std::string name;
    bool primitive;
  };
} // namespace riddle
