#pragma once

#include "riddle_export.h"
#include "field.h"
#include <map>

namespace riddle
{
  class core;

  class scope
  {
  public:
    RIDDLE_EXPORT scope(scope &scp);
    virtual ~scope() = default;

    scope &get_scope() { return scp; }

  private:
    scope &scp;
    std::map<std::string, field_ptr> fields;
  };
} // namespace riddle
