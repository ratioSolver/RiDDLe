#pragma once

#include "memory.h"
#include <string>

namespace riddle
{
  class type;

  class field
  {
  public:
    field(type &tp, const std::string &name) : tp(tp), name(name) {}
    ~field() {}

    type &get_type() { return tp; }
    const std::string &get_name() const { return name; }

  private:
    type &tp;
    std::string name;
  };

  using field_ptr = utils::u_ptr<field>;
} // namespace riddle
