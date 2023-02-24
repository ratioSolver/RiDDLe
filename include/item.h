#pragma once

#include "scope.h"
#include "enum.h"
#include <map>

namespace riddle
{
  class type;
  class complex_type;
  class predicate;

  /**
   * @brief Base class for all items.
   *
   */
  class item : virtual public utils::countable
  {
  public:
    item(type &tp);
    virtual ~item() = default;

    /**
     * @brief Get the core this item is defined in.
     *
     * @return core& The core.
     */
    core &get_core();
    /**
     * @brief Get the core this item is defined in.
     *
     * @return const core& The core.
     */
    const core &get_core() const;

    /**
     * @brief Get the type of this item.
     *
     * @return type& The type of this item.
     */
    type &get_type() { return tp; }
    /**
     * @brief Get the type of this item.
     *
     * @return const type& The type of this item.
     */
    const type &get_type() const { return tp; }

  private:
    type &tp;
  };

  /**
   * @brief Base class for all complex items. A complex item is an item that can have sub-items.
   *
   */
  class complex_item : public item, public env, public utils::enum_val
  {
  public:
    complex_item(type &tp);
    virtual ~complex_item() = default;

    RIDDLE_EXPORT expr &get(const std::string &name) override;
  };

  /**
   * @brief Base class for all enum items. An enum item is an item that can have sub-items.
   *
   */
  class enum_item : public item, public env
  {
  public:
    enum_item(type &tp);
    virtual ~enum_item() = default;

    RIDDLE_EXPORT expr &get(const std::string &name) override;
  };

  /**
   * @brief A class for representing atoms.
   *
   */
  class atom_item : public complex_item
  {
  public:
    atom_item(predicate &p, bool is_fact = true);
    virtual ~atom_item() = default;

    bool is_fact() const { return is_fact_; }

  private:
    const bool is_fact_;
  };

  inline uintptr_t get_id(const item &itm) noexcept { return reinterpret_cast<uintptr_t>(&itm); }
} // namespace riddle
