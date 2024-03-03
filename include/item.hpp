#pragma once

#include "env.hpp"
#include "enum.hpp"

namespace riddle
{
  class type;

  class item : public utils::enum_val
  {
  public:
    item(type &t) : tp(t) {}
    virtual ~item() = default;

    /**
     * @brief Get the type of the item.
     *
     * @return type& The type of the item.
     */
    type &get_type() const { return tp; }

  private:
    type &tp;
  };

  class component : public item, public env
  {
  public:
    component(type &t, std::shared_ptr<env> parent = nullptr);
    virtual ~component() = default;
  };
} // namespace riddle
