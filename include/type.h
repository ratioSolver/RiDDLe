#pragma once

#include "scope.h"
#include "method.h"
#include "constructor.h"
#include <functional>

namespace riddle
{
  /**
   * @brief The base class for all types.
   *
   */
  class type
  {
  public:
    type(scope &scp, const std::string &name, bool primitive = false);
    virtual ~type() = default;

    /**
     * @brief Get the core this type is defined in.
     *
     * @return core& The core.
     */
    core &get_core() { return scp.get_core(); }
    /**
     * @brief Get the core this type is defined in.
     *
     * @return const core& The core.
     */
    const core &get_core() const { return scp.get_core(); }

    /**
     * @brief Get the scope this type is defined in.
     *
     * @return scope& The scope.
     */
    scope &get_scope() { return scp; }
    /**
     * @brief Get the scope this type is defined in.
     *
     * @return const scope& The scope.
     */
    const scope &get_scope() const { return scp; }

    const std::string &get_name() const { return name; }

    /**
     * @brief Check if this type is assignable from another type.
     *
     * @param other The other type.
     * @return true If this type is assignable from the other type.
     * @return false If this type is not assignable from the other type.
     */
    bool is_assignable_from(const type &other) const;
    /**
     * @brief Check if this type is primitive (i.e. a built-in type).
     *
     * @return true If this type is primitive.
     * @return false If this type is not primitive.
     */
    bool is_primitive() const { return primitive; }

    /**
     * @brief Create a new instance of this type.
     *
     * @return expr The new instance.
     */
    virtual expr new_instance() = 0;

    bool operator==(const type &other) const { return this == &other; }
    bool operator!=(const type &other) const { return this != &other; }

  protected:
    scope &scp;

  private:
    std::string name;
    bool primitive;
  };

  /**
   * @brief The type for boolean values.
   *
   */
  class bool_type final : public type
  {
  public:
    bool_type(core &cr);

    expr new_instance() override;
  };

  /**
   * @brief The type for integer values.
   *
   */
  class int_type final : public type
  {
  public:
    int_type(core &cr);

    expr new_instance() override;
  };

  /**
   * @brief The type for real values.
   *
   */
  class real_type final : public type
  {
  public:
    real_type(core &cr);

    expr new_instance() override;
  };

  /**
   * @brief The type for time values.
   *
   */
  class time_point_type final : public type
  {
  public:
    time_point_type(core &cr);

    expr new_instance() override;
  };

  /**
   * @brief The type for string values.
   *
   */
  class string_type final : public type
  {
  public:
    string_type(core &cr);

    expr new_instance() override;
  };

  /**
   * @brief The type for a typedef.
   *
   */
  class typedef_type final : public type
  {
  public:
    typedef_type(scope &scp, const std::string &name, type &tp, const ast::expression_ptr &xpr);

    /**
     * @brief Get the basic type of this typedef.
     *
     * @return type& The basic type.
     */
    type &get_basic_type() const { return tp; }

    expr new_instance() override;

  private:
    type &tp;
    const ast::expression_ptr &xpr;
  };

  /**
   * @brief The type for an enum. It contains a set of allowed values representing the initial domain of the instantiated variables.
   *
   */
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

  /**
   * @brief The predicate type. A predicate is represented by a name and a set of arguments. It also contains a set of instances, which are the facts and goals that have been created from this predicate. Finally, it contains the body of the predicate, which is a set of statements that are executed when a goal has to be achieved.
   *
   */
  class predicate : public scope, public type
  {
    friend class core;
    friend class ast::predicate_declaration;

  public:
    RIDDLE_EXPORT predicate(scope &scp, const std::string &name, std::vector<field_ptr> args, const std::vector<ast::statement_ptr> &body);
    virtual ~predicate() = default;

    const std::vector<std::reference_wrapper<predicate>> &get_parents() const { return parents; }
    const std::vector<std::reference_wrapper<field>> &get_args() const { return args; }

    /**
     * @brief Create a new instance of this predicate. This is equivalent to calling new_fact().
     *
     * @return expr The new instance.
     */
    expr new_instance() override { return new_fact(); }
    const std::vector<expr> &get_instances() const { return instances; }

    /**
     * @brief Create a new fact from this predicate.
     *
     * @return expr The new fact.
     */
    expr new_fact();
    /**
     * @brief Create a new goal from this predicate.
     *
     * @return expr The new goal.
     */
    expr new_goal();

    /**
     * @brief Call the rule associated with this predicate with the given atom as the right-hand side.
     *
     * @param atm The atom representing the right-hand side of the rule.
     */
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
    bool has_type(const std::string &nm) const override { return types.find(nm) != types.end(); }
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
