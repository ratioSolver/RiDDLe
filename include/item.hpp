#pragma once

#include "env.hpp"
#include "enum.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class type;
  class bool_type;
  class int_type;
  class real_type;
  class time_type;
  class string_type;
  class enum_type;
  class component_type;

  /**
   * @class item item.hpp "include/item.hpp"
   * @brief The item class.
   *
   * The item class is the base class for all items in the RiDDLe language. An item
   * is a named entity that has a type. Items are stored in environments and can be
   * retrieved by their name.
   */
  class item : public utils::enum_val
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
    arith_item(int_type &t, const utils::lin &l);
    arith_item(real_type &t, const utils::lin &l);
    arith_item(time_type &t, const utils::lin &l);

    [[nodiscard]] utils::lin &get_value() { return value; }
    [[nodiscard]] const utils::lin &get_value() const { return value; }

  private:
    utils::lin value;
  };

  class string_item final : public item
  {
  public:
    string_item(string_type &tp, std::string &&s);

    [[nodiscard]] std::string &get_value() { return value; }
    [[nodiscard]] const std::string &get_value() const { return value; }

  private:
    std::string value;
  };

  class enum_item final : public item
  {
  public:
    enum_item(type &tp, utils::var v);

    [[nodiscard]] utils::var &get_value() { return value; }
    [[nodiscard]] const utils::var &get_value() const { return value; }

  private:
    utils::var value;
  };

  class component : public item, public env
  {
  public:
    component(component_type &t);
    virtual ~component() = default;
  };
} // namespace riddle
