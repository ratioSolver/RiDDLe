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
    /**
     * @brief Get the enclosing scope.
     *
     * @return scope& The enclosing scope.
     */
    scope &get_scope() { return scp; }

    /**
     * @brief Get the field with the given name.
     *
     * @param name The name of the field.
     * @return field& The field with the given name.
     */
    RIDDLE_EXPORT field &get_field(const std::string &name);

    /**
     * @brief Check if the scope has a field with the given name.
     *
     * @param name The name of the field.
     * @return true If the scope has a field with the given name.
     * @return false If the scope has no field with the given name.
     */
    virtual bool has_type(const std::string &name) const { return scp.has_type(name); }
    /**
     * @brief Get the type with the given name.
     *
     * @param name The name of the type.
     * @return type& The type with the given name.
     */
    virtual type &get_type(const std::string &name) { return scp.get_type(name); }

    /**
     * @brief Get the method with the given name and arguments.
     *
     * @param name The name of the method.
     * @param args The arguments of the method.
     * @return method& The method with the given name and arguments.
     */
    virtual method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) { return scp.get_method(name, args); }

    /**
     * @brief Get the predicate with the given name.
     *
     * @param name The name of the predicate.
     * @return predicate& The predicate with the given name.
     */
    virtual predicate &get_predicate(const std::string &name) { return scp.get_predicate(name); }

  protected:
    /**
     * @brief Add a field to the scope.
     *
     * @param f The field to add.
     */
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

    /**
     * @brief Returns the expression with the given name.
     *
     * @param name The name of the expression to get.
     * @return expr& The expression with the given name.
     */
    RIDDLE_EXPORT virtual expr &get(const std::string &name);

    /**
     * @brief Get all the variables in the environment.
     *
     * @return const std::map<std::string, expr>& The variables in the environment.
     */
    const std::map<std::string, expr> &get_vars() const { return items; }

  private:
    env_ptr parent;
    std::map<std::string, expr> items;
  };
} // namespace riddle
