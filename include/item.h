#pragma once

#include "scope.h"
#include <map>

namespace riddle
{
  class type;
  class complex_type;
  class predicate;

  class item : virtual public utils::countable
  {
  public:
    item(type &tp);
    virtual ~item() = default;

    type &get_type() { return tp; }

  private:
    type &tp;
  };

  class complex_item : public item
  {
  public:
    complex_item(type &tp);
    virtual ~complex_item() = default;

  private:
    std::map<std::string, expr> items;
  };
} // namespace riddle
