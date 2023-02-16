#pragma once

#include "riddle_export.h"
#include "field.h"
#include <vector>
#include <map>

namespace riddle
{
  class core;
  class type;
  class method;

  class scope
  {
  public:
    RIDDLE_EXPORT scope(scope &scp);
    virtual ~scope() = default;

    scope &get_scope() { return scp; }

    RIDDLE_EXPORT field &get_field(const std::string &name);

    virtual type &get_type(const std::string &name) { return scp.get_type(name); }

    virtual method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) { return scp.get_method(name, args); }

  private:
    scope &scp;
    std::map<std::string, field_ptr> fields;
  };
} // namespace riddle
