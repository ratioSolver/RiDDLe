#pragma once

#include "scope.h"
#include "method.h"
#include "constructor.h"
#include <functional>

namespace riddle
{
  class type
  {
  public:
    type(scope &scp, const std::string &name, bool primitive = false);
    virtual ~type() = default;

    const std::string &get_name() const { return name; }

    bool is_assignable_from(const type &other) const;
    bool is_primitive() const { return primitive; }

    virtual expr new_instance() = 0;

    bool operator==(const type &other) const { return this == &other; }
    bool operator!=(const type &other) const { return this != &other; }

  protected:
    scope &scp;

  private:
    std::string name;
    bool primitive;
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

  class time_point_type final : public type
  {
  public:
    time_point_type(core &cr);

    expr new_instance() override;
  };

  class string_type final : public type
  {
  public:
    string_type(core &cr);

    expr new_instance() override;
  };

  class typedef_type final : public type
  {
  public:
    typedef_type(scope &scp, const std::string &name, type &tp, const ast::expression_ptr &xpr);

    type &get_basic_type() const { return tp; }

    expr new_instance() override;

  private:
    type &tp;
    const ast::expression_ptr &expr;
  };

  class enum_type final : public type
  {
    friend class ast::enum_declaration;

  public:
    enum_type(scope &scp, const std::string &name);

    const std::vector<std::reference_wrapper<enum_type>> &get_enums() const { return enums; }

    std::vector<expr> get_all_values() const;

    expr new_instance() override;

  private:
    std::vector<expr> instances;
    std::vector<std::reference_wrapper<enum_type>> enums;
  };

  class predicate : public scope, public type
  {
    friend class ast::predicate_declaration;

  public:
    RIDDLE_EXPORT predicate(scope &scp, const std::string &name, std::vector<field_ptr> args, const std::vector<ast::statement_ptr> &body);
    virtual ~predicate() = default;

    const std::vector<std::reference_wrapper<predicate>> &get_parents() const { return parents; }
    const std::vector<std::reference_wrapper<field>> &get_args() const { return args; }

    expr new_instance() override { return new_fact(); }
    const std::vector<expr> &get_instances() const { return instances; }

    expr new_fact();
    expr new_goal();

    RIDDLE_EXPORT void call(expr &atm);

  protected:
    void add_parent(predicate &parent) { parents.emplace_back(parent); }

    void add_arg(field_ptr arg)
    {
      args.push_back(*arg);
      add_field(std::move(arg));
    }

  private:
    std::vector<std::reference_wrapper<predicate>> parents; // the base predicates (i.e. the predicates this predicate inherits from)..
    std::vector<std::reference_wrapper<field>> args;
    const std::vector<ast::statement_ptr> &body;
    std::vector<expr> instances;
  };

  class complex_type : public scope, public type
  {
    friend class ast::method_declaration;
    friend class ast::constructor_declaration;
    friend class ast::predicate_declaration;
    friend class ast::typedef_declaration;
    friend class ast::enum_declaration;
    friend class ast::class_declaration;

  public:
    RIDDLE_EXPORT complex_type(scope &scp, const std::string &name);
    virtual ~complex_type() = default;

    std::vector<std::reference_wrapper<complex_type>> get_parents() const { return parents; }

    RIDDLE_EXPORT constructor &get_constructor(const std::vector<std::reference_wrapper<type>> &args);
    bool has_type(const std::string &name) const override { return types.find(name) != types.end(); }
    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;
    RIDDLE_EXPORT predicate &get_predicate(const std::string &name) override;

    expr new_instance() override;
    const std::vector<expr> &get_instances() const { return instances; }

  protected:
    void add_parent(complex_type &parent) { parents.emplace_back(parent); }
    void add_constructor(constructor_ptr &&constructor) { constructors.emplace_back(std::move(constructor)); }
    void add_type(type_ptr &&type) { types.emplace(type->get_name(), std::move(type)); }
    void add_method(method_ptr &&method) { methods[method->get_name()].emplace_back(std::move(method)); }
    void add_predicate(predicate_ptr &&predicate) { predicates.emplace(predicate->get_name(), std::move(predicate)); }

  private:
    std::vector<std::reference_wrapper<complex_type>> parents; // the base types (i.e. the types this type inherits from)..
    std::vector<constructor_ptr> constructors;                 // the constructors for this type..
    std::map<std::string, type_ptr> types;                     // the types defined in this type..
    std::map<std::string, std::vector<method_ptr>> methods;    // the methods defined in this type..
    std::map<std::string, predicate_ptr> predicates;           // the predicates defined in this type..
    std::vector<expr> instances;                               // the instances of this type..
  };
} // namespace riddle
