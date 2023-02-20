#pragma once

#include "riddle_export.h"
#include "field.h"
#include <vector>
#include <map>

namespace riddle
{
  class core;
  class complex_item;

  class scope
  {
    friend class ast::field_declaration;

  public:
    RIDDLE_EXPORT scope(scope &scp);
    virtual ~scope() = default;

    virtual core &get_core() { return scp.get_core(); }
    virtual const core &get_core() const { return scp.get_core(); }
    scope &get_scope() { return scp; }

    RIDDLE_EXPORT field &get_field(const std::string &name);

    virtual bool has_type(const std::string &name) const { return scp.has_type(name); }
    virtual type &get_type(const std::string &name) { return scp.get_type(name); }

    virtual method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) { return scp.get_method(name, args); }

    virtual predicate &get_predicate(const std::string &name) { return scp.get_predicate(name); }

  protected:
    RIDDLE_EXPORT void add_field(field_ptr f);

  private:
    scope &scp;
    std::map<std::string, field_ptr> fields;
  };

  class env : virtual public utils::countable
  {
    friend class core;
    friend class complex_item;
    friend class constructor;
    friend class method;
    friend class ast::local_field_statement;
    friend class ast::assignment_statement;
    friend class ast::formula_statement;
    friend class ast::return_statement;

  public:
    RIDDLE_EXPORT env(env_ptr parent = nullptr);
    virtual ~env() = default;

    RIDDLE_EXPORT virtual expr &get(const std::string &name);

  private:
    env_ptr parent;
    std::map<std::string, expr> items;
  };
} // namespace riddle
