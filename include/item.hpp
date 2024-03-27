#pragma once

#include "env.hpp"
#include "enum.hpp"

namespace riddle
{
  class type;
  class predicate;

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
    [[nodiscard]] type &get_type() const { return tp; }

  private:
    type &tp;
  };

  class component : public item, public env
  {
  public:
    component(type &t, std::shared_ptr<env> parent = nullptr);
    virtual ~component() = default;
  };

  class atom : public item, public env
  {
  public:
    atom(predicate &t, bool is_fact, std::map<std::string, std::shared_ptr<item>> &&args = {});
    virtual ~atom() = default;

    [[nodiscard]] bool is_fact() const { return fact; }

  private:
    bool fact; // whether the atom is a fact or a goal..
  };
} // namespace riddle
