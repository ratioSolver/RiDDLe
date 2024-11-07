#pragma once

#include <vector>
#include "scope.hpp"
#include "constructor.hpp"
#include "method.hpp"

namespace riddle
{
  class constructor;
  class statement;
  class constructor_declaration;
  class method_declaration;
  class class_declaration;
  class typedef_declaration;
  class enum_declaration;
  class predicate_declaration;

  /**
   * @brief The base class for all types.
   */
  class type
  {
  public:
    type(scope &scp, std::string_view name, bool primitive = false);
    virtual ~type() = default;

    /**
     * @brief Get the scope of the type.
     *
     * @return scope& The scope of the type.
     */
    [[nodiscard]] scope &get_scope() const { return scp; }

    /**
     * @brief Get the name of the type.
     *
     * @return const std::string& The name of the type.
     */
    [[nodiscard]] const std::string &get_name() const { return name; }

    /**
     * @brief Check if this type is primitive (i.e. a built-in type).
     *
     * @return true If this type is primitive.
     * @return false If this type is not primitive.
     */
    [[nodiscard]] bool is_primitive() const { return primitive; }

    /**
     * @brief Check if this type is assignable from another type.
     *
     * @param other The other type.
     * @return true If this type is assignable from the other type.
     * @return false If this type is not assignable from the other type.
     */
    [[nodiscard]] virtual bool is_assignable_from(const type &other) const { return this == &other; }

    /**
     * @brief Create a new instance of this type.
     *
     * @return std::shared_ptr<item> The new instance.
     */
    [[nodiscard]] virtual std::shared_ptr<item> new_instance() = 0;

  protected:
    scope &scp;

  private:
    const std::string name;
    bool primitive;
  };

  /**
   * @brief The built-in boolean type.
   */
  class bool_type final : public type
  {
  public:
    bool_type(core &c);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in integer type.
   */
  class int_type final : public type
  {
  public:
    int_type(core &c);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in real type.
   */
  class real_type final : public type
  {
  public:
    real_type(core &c);

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in time type.
   */
  class time_type final : public type
  {
  public:
    time_type(core &c);

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in string type.
   */
  class string_type final : public type
  {
  public:
    string_type(core &c);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;
  };

  class typedef_type final : public type
  {
  public:
    typedef_type(scope &parent, std::string_view name, type &base_type, expression &value);

    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    type &base_type;
    expression &value;
  };

  class enum_type final : public type
  {
    friend class enum_declaration;

  public:
    enum_type(scope &parent, std::string_view name, std::vector<std::shared_ptr<item>> &&values);

    [[nodiscard]] std::vector<std::shared_ptr<item>> get_values() const;
    [[nodiscard]] std::vector<std::reference_wrapper<enum_type>> get_enums() const { return enums; }

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    std::vector<std::shared_ptr<item>> values;
    std::vector<std::reference_wrapper<enum_type>> enums;
  };

  /**
   * @brief The component type.
   */
  class component_type : public type, public scope
  {
    friend class core;
    friend class constructor;
    friend class constructor_declaration;
    friend class method_declaration;
    friend class class_declaration;
    friend class typedef_declaration;
    friend class enum_declaration;
    friend class predicate_declaration;

  public:
    component_type(scope &parent, std::string_view name);
    virtual ~component_type() = default;

    /**
     * @brief Get the parent types.
     *
     * @return const std::vector<std::reference_wrapper<component_type>>& The parent types.
     */
    [[nodiscard]] const std::vector<std::reference_wrapper<component_type>> &get_parents() const { return parents; }

    [[nodiscard]] std::optional<std::reference_wrapper<field>> get_field(std::string_view name) const noexcept override;

    /**
     * @brief Get the constructors of the type that match the given argument types.
     *
     * @param argument_types The argument types.
     * @return std::optional<std::reference_wrapper<constructor>> The constructor, if found.
     */
    [[nodiscard]] std::optional<std::reference_wrapper<constructor>> get_constructor(const std::vector<std::reference_wrapper<const type>> &argument_types) const;

    [[nodiscard]] std::optional<std::reference_wrapper<method>> get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const override;

    [[nodiscard]] std::optional<std::reference_wrapper<type>> get_type(std::string_view name) const override;
    [[nodiscard]] std::vector<std::reference_wrapper<type>> get_types() const
    {
      std::vector<std::reference_wrapper<type>> tps;
      for (const auto &tp : types)
        tps.emplace_back(*tp.second);
      return tps;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<predicate>> get_predicate(std::string_view name) const override;
    [[nodiscard]] std::vector<std::reference_wrapper<predicate>> get_predicates() const
    {
      std::vector<std::reference_wrapper<predicate>> preds;
      for (const auto &pred : predicates)
        preds.emplace_back(*pred.second);
      return preds;
    }

    /**
     * @brief Get the instances of the type.
     *
     * @return const std::vector<std::shared_ptr<item>>& The instances of the type.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<item>> &get_instances() const { return instances; }

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  protected:
    /**
     * Adds a parent component to the current component.
     *
     * @param parent The parent component to be added.
     */
    void add_parent(component_type &parent);

    /**
     * Adds a parent predicate to a child predicate.
     *
     * @param child The child predicate to add a parent to.
     * @param parent The parent predicate to be added.
     */
    static void add_parent(predicate &child, predicate &parent);

    /**
     * @brief Adds a constructor to this component type.
     *
     * This function adds a constructor to the collection of constructors. The constructor is passed as a unique pointer
     * and is moved into the collection.
     *
     * @param ctor A unique pointer to the constructor to be added.
     */
    void add_constructor(std::unique_ptr<constructor> &&ctor);
    /**
     * @brief Adds a method to this component type.
     *
     * @param mthd A unique pointer to the method to be added.
     */
    void add_method(std::unique_ptr<method> &&mthd);
    /**
     * @brief Adds a type to this component type.
     *
     * This function adds a type to the collection of types.
     *
     * @param tp A unique pointer to the type to be added.
     */
    void add_type(std::unique_ptr<type> &&tp);
    /**
     * @brief Adds a predicate to this component type.
     *
     * This function adds a predicate to the collection of predicates.
     *
     * @param pred A unique pointer to the predicate to be added.
     */
    void add_predicate(std::unique_ptr<predicate> &&pred);

  private:
    virtual void new_predicate([[maybe_unused]] predicate &pred) {}

  private:
    std::vector<std::reference_wrapper<component_type>> parents;                      // the base types (i.e. the types this type inherits from)..
    std::vector<std::unique_ptr<constructor>> constructors;                           // the constructors of the type..
    std::map<std::string, std::vector<std::unique_ptr<method>>, std::less<>> methods; // the methods declared in the scope of the type..
    std::map<std::string, std::unique_ptr<type>, std::less<>> types;                  // the types declared in the scope of the type..
    std::map<std::string, std::unique_ptr<predicate>, std::less<>> predicates;        // the predicates declared in the scope of the type..
    std::vector<std::shared_ptr<item>> instances;                                     // the instances of the type..
  };

  class predicate : public type, public scope
  {
    friend class core;
    friend class component_type;
    friend class predicate_declaration;

  public:
    predicate(scope &parent, std::string_view name, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body);
    virtual ~predicate() = default;

    /**
     * @brief Get the parent predicates.
     *
     * @return const std::vector<std::reference_wrapper<predicate>>& The parent predicates.
     */
    [[nodiscard]] const std::vector<std::reference_wrapper<predicate>> &get_parents() const { return parents; }

    [[nodiscard]] std::optional<std::reference_wrapper<field>> get_field(std::string_view name) const noexcept override;

    /**
     * @brief Get the atoms of the predicate.
     *
     * @return const std::vector<std::shared_ptr<atom>>& The atoms of the predicate.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<atom>> &get_atoms() const { return atoms; }

    [[nodiscard]] bool is_assignable_from(const type &other) const override;

    /**
     * @brief Call the rule associated with this predicate with the given atom as the left-hand side.
     *
     * A rule is a formula of the form `p(x1, ..., xn) <- s1, ..., sm.` where `p` is a predicate, `x1, ..., xn` are the arguments of the predicate, and `s1, ..., sm` are the statements in the body of the rule.
     * When a rule is fired, the statements in the body are executed and the goal `p(x1, ..., xn)` is achieved.
     *
     * @param atm The atom.
     */
    void call(std::shared_ptr<atom> atm);

  protected:
    /**
     * Adds a parent predicate to the current predicate.
     *
     * @param parent The parent predicate to be added.
     */
    void add_parent(predicate &parent);

  private:
    [[nodiscard]] std::shared_ptr<item> new_instance() override;

  private:
    std::vector<std::reference_wrapper<predicate>> parents; // the base predicates (i.e. the predicates this predicate inherits from)..
    std::vector<std::reference_wrapper<field>> args;        // the arguments of the predicate..
    const std::vector<std::unique_ptr<statement>> &body;    // the body of the predicate..
    std::vector<std::shared_ptr<atom>> atoms;               // the atoms having this predicate as their predicate..
  };
} // namespace riddle
