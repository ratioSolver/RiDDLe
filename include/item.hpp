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
  class predicate;

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
    item(const item &) = delete;

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

  class bool_item : public item
  {
  public:
    bool_item(bool_type &tp);
    virtual ~bool_item() = default;
  };

  class arith_item : public item
  {
  public:
    arith_item(int_type &tp);
    arith_item(real_type &tp);
    arith_item(time_type &tp);
    virtual ~arith_item() = default;
  };

  class string_item : public item
  {
  public:
    string_item(string_type &tp);
    virtual ~string_item() = default;
  };

  class enum_item final : public item
  {
  public:
    enum_item(type &tp);
    virtual ~enum_item() = default;
  };

  class component : public item, public env
  {
  public:
    component(component_type &tp);
    virtual ~component() = default;
  };

  class atom : public item, public env
  {
  public:
    atom(predicate &t, bool fact, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args = {});
    virtual ~atom() = default;

    [[nodiscard]] bool is_fact() const { return fact; }

  private:
    static env &atom_parent(const predicate &t, const std::map<std::string, std::shared_ptr<item>, std::less<>> &args);

  private:
    bool fact; // whether the atom is a fact
  };
} // namespace riddle
