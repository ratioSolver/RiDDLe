#pragma once

#include "env.hpp"
#include "enum.hpp"
#include <vector>

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
  class bool_item;

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
    virtual ~item() = default;

    /**
     * @brief Get the type of the item.
     *
     * This function returns a reference to the type of the item.
     *
     * @return A reference to the type of the item.
     */
    [[nodiscard]] type &get_type() const { return tp; }

    [[nodiscard]] virtual std::shared_ptr<bool_item> operator==(std::shared_ptr<item> rhs) const = 0;

    [[nodiscard]] virtual json::json to_json() const = 0;

    /**
     * @brief Get the unique identifier of the item.
     *
     * This function returns the unique identifier of the item.
     *
     * @return The unique identifier of the item.
     */
    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

  private:
    type &tp;
  };

  using expr = std::shared_ptr<item>;

  class bool_item : public item
  {
  public:
    bool_item(bool_type &tp);
    virtual ~bool_item() = default;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using bool_expr = std::shared_ptr<bool_item>;

  class arith_item : public item
  {
  public:
    arith_item(int_type &tp);
    arith_item(real_type &tp);
    arith_item(time_type &tp);
    virtual ~arith_item() = default;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using arith_expr = std::shared_ptr<arith_item>;

  class string_item : public item
  {
  public:
    string_item(string_type &tp);
    virtual ~string_item() = default;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  using string_expr = std::shared_ptr<string_item>;

  class enum_item : public item
  {
  public:
    enum_item(type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values);
    virtual ~enum_item() = default;

    [[nodiscard]] const std::vector<std::reference_wrapper<utils::enum_val>> &get_values() const noexcept { return values; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    std::vector<std::reference_wrapper<utils::enum_val>> values;
  };

  using enum_expr = std::shared_ptr<enum_item>;

  class component : public item, public env
  {
  public:
    component(component_type &tp);
    virtual ~component() = default;

    [[nodiscard]] std::shared_ptr<bool_item> operator==(std::shared_ptr<item> rhs) const override;

    [[nodiscard]] virtual json::json to_json() const override;
  };

  enum atom_state
  {
    active,   // the atom is active
    inactive, // the atom is inactive
    unified   // the atom is unified
  };

  class atom : public item, public env
  {
  public:
    atom(predicate &t, bool fact, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args = {});
    virtual ~atom() = default;

    [[nodiscard]] bool is_fact() const { return fact; }

    [[nodiscard]] virtual atom_state get_state() const { return atom_state::active; }

    [[nodiscard]] virtual json::json to_json() const override;

  private:
    static env &atom_parent(const predicate &t, const std::map<std::string, std::shared_ptr<item>, std::less<>> &args);

  private:
    bool fact; // whether the atom is a fact
  };

  using atom_expr = std::shared_ptr<atom>;
} // namespace riddle
