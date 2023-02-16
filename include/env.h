#pragma once

#include "riddle_export.h"
#include "memory.h"
#include <string>

namespace riddle
{
  class core;
  class item;
  using expr = utils::c_ptr<item>;

  class env
  {
  public:
    RIDDLE_EXPORT env(env &e);
    virtual ~env() = default;

    env &get_env() { return e; }

    virtual expr get(const std::string &name) const { return e.get(name); }

  private:
    env &e;
  };
} // namespace riddle
