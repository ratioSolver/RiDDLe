#pragma once

#include "env.h"
#include <map>

namespace riddle
{
  class type;

  class item : public utils::countable, public env
  {
  public:
    item(env &e, type &tp);
    virtual ~item() = default;

    type &get_type() { return tp; }

  private:
    type &tp;
  };

  class complex_item : public item
  {
  public:
    complex_item(env &e, type &tp);
    virtual ~complex_item() = default;

    expr get(const std::string &name) const override;

  private:
    std::map<std::string, expr> items;
  };
} // namespace riddle
