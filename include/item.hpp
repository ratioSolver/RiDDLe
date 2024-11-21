#pragma once

#include "env.hpp"
#include "enum.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class type;

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
} // namespace riddle
