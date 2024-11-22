#pragma once

#include "env.hpp"
#include "enum.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class type;
  class bool_type;

  /**
   * @class item item.hpp "include/item.hpp"
   * @brief The item class.
   *
   * The item class is the base class for all items in the RiDDLe language. An item
   * is a named entity that has a type. Items are stored in environments and can be
   * retrieved by their name.
   */
  class item
  {
  public:
    item(type &tp) noexcept : tp(tp) {}

    /**
     * @brief Get the type of the item.
     *
     * This function returns a reference to the type of the item.
     *
     * @return A reference to the type of the item.
     */
    [[nodiscard]] type &get_type() const { return tp; }

  private:
    type &tp;
  };

  class bool_item final : public item
  {
  public:
    bool_item(bool_type &tp, const utils::lit &l);

    [[nodiscard]] utils::lit &get_value() { return value; }
    [[nodiscard]] const utils::lit &get_value() const { return value; }

  private:
    utils::lit value;
  };

  class arith_item final : public item
  {
  public:
    arith_item(type &tp, const utils::lin &l);

    [[nodiscard]] utils::lin &get_value() { return value; }
    [[nodiscard]] const utils::lin &get_value() const { return value; }

  private:
    utils::lin value;
  };
} // namespace riddle
