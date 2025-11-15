#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace riddle
{
  class core;
  class method;
  class type;
  class predicate;
  class expression;
  class field_declaration;

  /**
   * @brief A field.
   */
  class field
  {
  public:
    field(type &tp, std::string &&name, const std::unique_ptr<expression> &expr) noexcept;

    /**
     * @brief Retrieves the type of the field.
     *
     * This function returns a reference to the type of the field.
     *
     * @return A reference to the type of the field.
     */
    [[nodiscard]] type &get_type() const noexcept { return tp; }

    /**
     * @brief Retrieves the name of the field.
     *
     * This function returns a reference to the name of the field.
     *
     * @return A reference to the name of the field.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }

    /**
     * @brief Retrieves the expression of the field.
     *
     * This function returns a reference to the expression of the field.
     *
     * @return A reference to the expression of the field.
     */
    [[nodiscard]] const std::unique_ptr<expression> &get_expression() const noexcept { return expr; }

  private:
    type &tp;
    std::string name;
    const std::unique_ptr<expression> &expr;
  };

  class scope
  {
    friend class field_declaration;
    friend class local_field_statement;

  public:
    scope(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args = {});
    scope(const scope &) = delete;
    virtual ~scope() = default;

    [[nodiscard]] core &get_core() const noexcept { return cr; }
    /**
     * @brief Retrieves the parent scope.
     *
     * This function returns a reference to the parent scope of the current scope.
     *
     * @return A reference to the parent scope.
     */
    [[nodiscard]] scope &get_parent() const noexcept { return parent; }

    /**
     * @brief Retrieves the fields of the current scope.
     *
     * This function returns a constant reference to a map containing the fields
     * of the current scope. The map keys are strings representing the field names,
     * and the values are unique pointers to the field objects.
     *
     * @return A constant reference to a map with field names as keys and unique pointers to fields as values.
     */
    [[nodiscard]] const std::map<std::string, std::unique_ptr<field>, std::less<>> &get_fields() const noexcept { return fields; }

    /**
     * @brief Retrieves the field associated with the given name.
     *
     * This function searches for the field associated with the provided name
     * within the current scope. If the field is not found in the current scope,
     * it delegates the search to the parent scope.
     *
     * @param name The name of the field to retrieve.
     * @return field& A reference to the field associated with the given name.
     * @throws std::out_of_range if the field is not found in the current or parent scope.
     */
    [[nodiscard]] virtual field &get_field(std::string_view name) const;

    [[nodiscard]] virtual const std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> &get_methods() const { return parent.get_methods(); }

    /**
     * @brief Retrieves the method associated with the given name and argument types.
     *
     * This function searches for the method associated with the provided name and argument types
     * within the current scope. If the method is not found in the current scope,
     * it delegates the search to the parent scope.
     *
     * @param name The name of the method to retrieve.
     * @param argument_types A vector of references to the types of the arguments of the method.
     * @return method& A reference to the method associated with the given name and argument types.
     * @throws std::out_of_range if the method is not found in the current or parent scope.
     */
    [[nodiscard]] virtual method &get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const { return parent.get_method(name, argument_types); }

    [[nodiscard]] virtual const std::map<std::string, std::unique_ptr<type>, std::less<>> &get_types() const { return parent.get_types(); }

    /**
     * @brief Retrieves the type associated with the given name.
     *
     * This function searches for the type associated with the provided name
     * within the current scope. If the type is not found in the current scope,
     * it delegates the search to the parent scope.
     *
     * @param name The name of the type to retrieve.
     * @return type& A reference to the type associated with the given name.
     * @throws std::out_of_range if the type is not found in the current or parent scope.
     */
    [[nodiscard]] virtual type &get_type(std::string_view name) const { return parent.get_type(name); }

    [[nodiscard]] virtual const std::map<std::string, std::unique_ptr<predicate>, std::less<>> &get_predicates() const { return parent.get_predicates(); }

    /**
     * @brief Retrieves the predicate associated with the given name.
     *
     * This function searches for the predicate associated with the provided name
     * within the current scope. If the predicate is not found in the current scope,
     * it delegates the search to the parent scope.
     *
     * @param name The name of the predicate to retrieve.
     * @return predicate& A reference to the predicate associated with the given name.
     * @throws std::out_of_range if the predicate is not found in the current or parent scope.
     */
    [[nodiscard]] virtual predicate &get_predicate(std::string_view name) const { return parent.get_predicate(name); }

  protected:
    /**
     * @brief Adds a field to the scope.
     *
     * This function takes ownership of the provided field and adds it to the scope.
     *
     * @param field A unique pointer to the field to be added.
     */
    void add_field(std::unique_ptr<field> field);

  private:
    core &cr;
    scope &parent;

  protected:
    std::map<std::string, std::unique_ptr<field>, std::less<>> fields;
  };
} // namespace riddle
