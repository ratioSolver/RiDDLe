#pragma once

#include "riddle_export.h"
#include "field.h"
#include <vector>
#include <map>

namespace riddle
{
  class core;
  class method;
  using method_ptr = utils::u_ptr<method>;
  class item;
  using expr = utils::c_ptr<item>;

  class scope
  {
  public:
    RIDDLE_EXPORT scope(scope &scp);
    virtual ~scope() = default;

    virtual core &get_core() { return scp.get_core(); }
    scope &get_scope() { return scp; }

    RIDDLE_EXPORT field &get_field(const std::string &name);

    virtual type &get_type(const std::string &name) { return scp.get_type(name); }

    virtual method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) { return scp.get_method(name, args); }

  protected:
    RIDDLE_EXPORT void add_field(field_ptr f);

  private:
    scope &scp;
    std::map<std::string, field_ptr> fields;
  };

  class env : virtual public utils::countable
  {
    friend class core;

  public:
    RIDDLE_EXPORT env(scope &scp, context ctx);
    virtual ~env() = default;

    env &get_context() { return *ctx; }

    type &get_type(const std::string &name) { return scp.get_type(name); }

    method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) { return scp.get_method(name, args); }

    virtual expr &get(const std::string &name);

  private:
    scope &scp;
    context ctx;
    std::map<std::string, expr> items;
  };
} // namespace riddle
