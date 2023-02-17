#pragma once

#include "scope.h"

namespace riddle
{
  class type
  {
  public:
    type(core &cr, const std::string &name);
    virtual ~type() = default;

    const std::string &get_name() const { return name; }

    bool is_assignable_from(const type &other) const;

    virtual expr new_instance() = 0;

    bool operator==(const type &other) const { return this == &other; }
    bool operator!=(const type &other) const { return this != &other; }

  protected:
    core &cr;

  private:
    std::string name;
  };

  class bool_type final : public type
  {
  public:
    bool_type(core &cr);

    expr new_instance() override;
  };

  class int_type final : public type
  {
  public:
    int_type(core &cr);

    expr new_instance() override;
  };

  class real_type final : public type
  {
  public:
    real_type(core &cr);

    expr new_instance() override;
  };

  class string_type final : public type
  {
  public:
    string_type(core &cr);

    expr new_instance() override;
  };

  class predicate : public scope, public type
  {
  public:
    RIDDLE_EXPORT predicate(scope &scp, const std::string &name);
    virtual ~predicate() = default;

    expr new_instance() override;

    expr new_fact();
    expr new_goal();

  private:
    std::vector<expr> instances;
  };

  class complex_type : public scope, public type
  {
  public:
    RIDDLE_EXPORT complex_type(scope &scp, const std::string &name);
    virtual ~complex_type() = default;

    std::vector<type *> get_supertypes() const { return supertypes; }

    RIDDLE_EXPORT constructor &get_constructor(const std::vector<std::reference_wrapper<type>> &args);
    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;

    expr new_instance() override;

  private:
    std::vector<type *> supertypes;                         // the base types (i.e. the types this type inherits from)..
    std::vector<constructor_ptr> constructors;              // the constructors for this type..
    std::map<std::string, type_ptr> types;                  // the types defined in this type..
    std::map<std::string, std::vector<method_ptr>> methods; // the methods defined in this type..
    std::vector<expr> instances;                            // the instances of this type..
  };
} // namespace riddle
