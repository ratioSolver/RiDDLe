#pragma once

#include "constructor.hpp"
#include "method.hpp"

namespace riddle
{
  class item;
  class predicate;
  class enum_declaration;
  class method_declaration;
  class class_declaration;
  class predicate_declaration;

  /**
   * @class type type.hpp "include/type.hpp"
   * @brief The type class.
   *
   * The type class is used to represent the type of an item. Each item has a type
   * that determines the kind of data it can store. Types can be primitive or
   * user-defined.
   */
  class type
  {
  public:
    type(scope &scp, std::string &&name, bool primitive = false) noexcept;
    type(const type &) = delete;
    virtual ~type() = default;

    /**
     * @brief Retrieves the scope where the type is defined.
     *
     * This function returns a reference to the scope where the type is defined.
     *
     * @return scope& A reference to the scope where the type is defined.
     */
    [[nodiscard]] scope &get_scope() const noexcept { return scp; }

    /**
     * @brief Retrieves the name of the type.
     *
     * This function returns a constant reference to the name of the type.
     *
     * @return const std::string& A constant reference to the name of the type.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }

    /**
     * @brief Checks if the type is primitive.
     *
     * This function returns a boolean value indicating whether the type
     * is considered primitive.
     *
     * @return true if the type is primitive, false otherwise.
     */
    [[nodiscard]] bool is_primitive() const noexcept { return primitive; }

    /**
     * @brief Checks if the current type is assignable from another type.
     *
     * This function returns a boolean value indicating whether the current type
     * is assignable from another type.
     *
     * @param other The type to compare with the current type.
     * @return true if the current type is assignable from the other type, false otherwise.
     */
    [[nodiscard]] virtual bool is_assignable_from(const type &other) const { return this == &other; }

    /**
     * @brief Creates a new instance of the item.
     *
     * This function is a pure virtual function that must be overridden by
     * derived classes to create and return a new instance of the item.
     *
     * @return std::shared_ptr<item> A shared pointer to the newly created item instance.
     */
    [[nodiscard]] virtual std::shared_ptr<item> new_instance() = 0;

  private:
    scope &scp;
    std::string name;
    bool primitive;
  };

  /**
   * @class bool_type type.hpp "include/type.hpp"
   * @brief The bool type class.
   *
   * The bool type class is used to represent the boolean type.
   */
  class bool_type final : public type
  {
  public:
    bool_type(core &cr) noexcept;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @class int_type type.hpp "include/type.hpp"
   * @brief The int type class.
   *
   * The int type class is used to represent the integer type.
   */
  class int_type final : public type
  {
  public:
    int_type(core &cr) noexcept;

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @class real_type type.hpp "include/type.hpp"
   * @brief The real type class.
   *
   * The real type class is used to represent the real number type.
   */
  class real_type final : public type
  {
  public:
    real_type(core &cr) noexcept;

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @class time_type type.hpp "include/type.hpp"
   * @brief The time type class.
   *
   * The time type class is used to represent the time type.
   */
  class time_type final : public type
  {
  public:
    time_type(core &cr) noexcept;

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @class string_type type.hpp "include/type.hpp"
   * @brief The string type class.
   *
   * The string type class is used to represent the string type.
   */
  class string_type final : public type
  {
  public:
    string_type(core &cr) noexcept;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @class enum_type type.hpp "include/type.hpp"
   * @brief The enum type class.
   *
   * The enum type class is used to represent the enumeration type.
   */
  class enum_type final : public type
  {
    friend class enum_declaration;

  public:
    enum_type(scope &scp, std::string &&name, std::vector<std::shared_ptr<item>> &&domain) noexcept;

    /**
     * @brief Retrieves the domain of items.
     *
     * This function returns a constant reference to a vector of shared pointers to items,
     * representing the domain.
     *
     * @return const std::vector<std::shared_ptr<item>>& A constant reference to the domain vector.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<item>> &get_domain() const noexcept { return domain; }
    [[nodiscard]] const std::vector<std::reference_wrapper<enum_type>> &get_enums() const noexcept { return enums; }

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    std::vector<std::shared_ptr<item>> domain;
    std::vector<std::reference_wrapper<enum_type>> enums;
  };

  /**
   * @class component_type type.hpp "include/type.hpp"
   * @brief The component type class.
   *
   * The component type class is used to represent the component type.
   */
  class component_type : public scope, public type
  {
    friend class enum_declaration;
    friend class method_declaration;
    friend class class_declaration;
    friend class predicate_declaration;
    friend class constructor_declaration;
    friend class constructor;
    friend class constructor_expression;

  public:
    component_type(scope &scp, std::string &&name) noexcept;
    virtual ~component_type() = default;

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

    [[nodiscard]] const std::vector<std::reference_wrapper<component_type>> &get_parents() const noexcept { return parents; }

    /**
     * @brief Retrieves a constructor that matches the given argument types.
     *
     * This function searches for a constructor that can be invoked with the specified
     * argument types and returns a reference to it.
     *
     * @param argument_types A vector of references to the types of the arguments that the
     *                       constructor should accept.
     * @return A reference to the constructor that matches the given argument types.
     * @throws std::out_of_range if the constructor is not found.
     */
    [[nodiscard]] constructor &get_constructor(const std::vector<std::reference_wrapper<const type>> &argument_types) const;

    [[nodiscard]] method &get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;
    [[nodiscard]] type &get_type(std::string_view name) const override;
    [[nodiscard]] predicate &get_predicate(std::string_view name) const override;

    [[nodiscard]] const std::map<std::string, std::unique_ptr<type>, std::less<>> &get_types() const noexcept { return types; }
    [[nodiscard]] const std::map<std::string, std::unique_ptr<predicate>, std::less<>> &get_predicates() const noexcept { return predicates; }

    /**
     * @brief retrieves the list of the instances of the type.
     *
     * This function returns a reference to the vector containing instances.
     *
     * @return std::vector<std::shared_ptr<item>>& A reference to the vector of instances.
     */
    [[nodiscard]] std::vector<std::shared_ptr<item>> &get_instances() noexcept { return instances; }

  protected:
    /**
     * @brief Adds a parent to this component type.
     *
     * This function adds a parent to the collection of parents.
     *
     * @param parent A reference to the parent to be added.
     */
    void add_parent(component_type &parent);

    /**
     * @brief Adds a constructor to this component type.
     *
     * This function adds a constructor to the collection of constructors.
     *
     * @param ctr A unique pointer to the constructor to be added.
     */
    void add_constructor(std::unique_ptr<constructor> ctr);

    /**
     * @brief Adds a method to this component type.
     *
     * This function takes ownership of the provided unique pointer to a method
     * and adds it to the internal collection of methods.
     *
     * @param mthd A unique pointer to the method to be added.
     */
    void add_method(std::unique_ptr<method> mthd);

    /**
     * @brief Adds a predicate to this component type.
     *
     * This function takes ownership of the provided predicate and adds it to the
     * internal collection of predicates.
     *
     * @param pred A unique pointer to the predicate to be added.
     */
    void add_predicate(std::unique_ptr<predicate> pred);

    /**
     * @brief Adds a type to this component type.
     *
     * This function takes ownership of the provided type and adds it to the
     * internal collection of types.
     *
     * @param tp A unique pointer to the type to be added.
     */
    void add_type(std::unique_ptr<type> tp);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    std::vector<std::reference_wrapper<component_type>> parents;                      // the base types (i.e. the types this type inherits from)..
    std::vector<std::unique_ptr<constructor>> constructors;                           // the constructors of the type..
    std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> methods; // the methods declared in the scope of the type..
    std::map<std::string, std::unique_ptr<type>, std::less<>> types;                  // the types declared in the scope of the type..
    std::map<std::string, std::unique_ptr<predicate>, std::less<>> predicates;        // the predicates declared in the scope of the type..
    std::vector<std::shared_ptr<item>> instances;                                     // the instances of the type..
  };

  /**
   * @class predicate type.hpp "include/type.hpp"
   * @brief The predicate class.
   *
   * The predicate class is used to represent a predicate.
   */
  class predicate : public scope, public type
  {
    friend class core;
    friend class predicate_declaration;

  public:
    predicate(scope &scp, std::string &&name, std::vector<std::unique_ptr<field>> &&args = {}, const std::vector<std::unique_ptr<statement>> &body = {}) noexcept;
    virtual ~predicate() = default;

    [[nodiscard]] const std::vector<std::reference_wrapper<predicate>> &get_parents() const noexcept { return parents; }

    /**
     * @brief Retrieves the arguments of the predicate.
     *
     * This function returns a constant reference to a vector of references to fields,
     * representing the arguments of the predicate.
     *
     * @return const std::vector<std::reference_wrapper<field>>& A constant reference to the arguments vector.
     */
    [[nodiscard]] const std::vector<std::reference_wrapper<field>> &get_args() const noexcept { return args; }

    /**
     * @brief Retrieves the list of atoms.
     *
     * This function returns a constant reference to the vector containing atoms.
     *
     * @return const std::vector<atom_expr>& A constant reference to the vector of atoms.
     */
    [[nodiscard]] const std::vector<atom_expr> &get_atoms() const noexcept { return atoms; }

    /**
     * @brief Calls the rule associated with the predicate to achieve the desired goal.
     *
     * This function calls the rule associated with the predicate to achieve the desired goal.
     *
     * @param atm A shared pointer to the atom to be called.
     */
    void call(atom_expr atm);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    std::vector<std::reference_wrapper<predicate>> parents; // the base predicates (i.e. the predicates this predicate inherits from)..
    std::vector<std::reference_wrapper<field>> args;        // the arguments of the predicate..
    const std::vector<std::unique_ptr<statement>> &body;    // the body of the predicate..
    std::vector<atom_expr> atoms;                           // the atoms of the predicate..
  };
} // namespace riddle
