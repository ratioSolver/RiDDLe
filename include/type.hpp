#pragma once

#include "scope.hpp"

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
    type(scope &scp, std::string &&name, bool primitive = false) noexcept;
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
    scope &scp;
    std::string name;
    bool primitive;
  };

  /**
   * @class bool_type type.hpp "include/type.hpp"
   * @brief The bool type class.
   *
   * The bool type class is used to represent the boolean type.
   */
  class bool_type final : public type
  {
  public:
    bool_type(core &cr) noexcept;
  };

  /**
   * @class int_type type.hpp "include/type.hpp"
   * @brief The int type class.
   *
   * The int type class is used to represent the integer type.
   */
  class int_type final : public type
  {
  public:
    int_type(core &cr) noexcept;
  };

  /**
   * @class real_type type.hpp "include/type.hpp"
   * @brief The real type class.
   *
   * The real type class is used to represent the real number type.
   */
  class real_type final : public type
  {
  public:
    real_type(core &cr) noexcept;
  };

  /**
   * @class time_type type.hpp "include/type.hpp"
   * @brief The time type class.
   *
   * The time type class is used to represent the time type.
   */
  class time_type final : public type
  {
  public:
    time_type(core &cr) noexcept;
  };

  /**
   * @class string_type type.hpp "include/type.hpp"
   * @brief The string type class.
   *
   * The string type class is used to represent the string type.
   */
  class string_type final : public type
  {
  public:
    string_type(core &cr) noexcept;
  };
} // namespace riddle
