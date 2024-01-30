#pragma once

#include <vector>
#include "scope.hpp"

namespace riddle
{
  class item;
  class constructor;

  /**
   * @brief The base class for all types.
   */
  class type
  {
  public:
    type(scope &scp, const std::string &name, bool primitive = false);
    virtual ~type() = default;

    /**
     * @brief Get the scope of the type.
     *
     * @return scope& The scope of the type.
     */
    scope &get_scope() const { return scp; }

    /**
     * @brief Get the name of the type.
     *
     * @return const std::string& The name of the type.
     */
    const std::string &get_name() const { return name; }

    /**
     * @brief Check if this type is primitive (i.e. a built-in type).
     *
     * @return true If this type is primitive.
     * @return false If this type is not primitive.
     */
    bool is_primitive() const { return primitive; }

    /**
     * @brief Check if this type is assignable from another type.
     *
     * @param other The other type.
     * @return true If this type is assignable from the other type.
     * @return false If this type is not assignable from the other type.
     */
    bool is_assignable_from(const type &other) const;

    /**
     * @brief Create a new instance of this type.
     *
     * @return std::shared_ptr<item> The new instance.
     */
    virtual std::shared_ptr<item> new_instance() = 0;

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
    std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in integer type.
   */
  class int_type final : public type
  {
  public:
    int_type(core &c);

  private:
    std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in real type.
   */
  class real_type final : public type
  {
  public:
    real_type(core &c);

  private:
    std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in time type.
   */
  class time_type final : public type
  {
  public:
    time_type(core &c);

  private:
    std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The built-in string type.
   */
  class string_type final : public type
  {
  public:
    string_type(core &c);

  private:
    std::shared_ptr<item> new_instance() override;
  };

  /**
   * @brief The component type.
   */
  class component_type : public type, public scope
  {
  public:
    component_type(std::shared_ptr<scope> parent, const std::string &name);
    virtual ~component_type() = default;

    /**
     * @brief Get the parent types.
     *
     * @return const std::vector<std::shared_ptr<component_type>>& The parent types.
     */
    const std::vector<std::shared_ptr<component_type>> &get_parents() const { return parents; }

    /**
     * @brief Get the constructors of the type that match the given argument types.
     *
     * @param argument_types The argument types.
     * @return constructor& The constructor.
     */
    constructor &get_constructor(const std::vector<std::reference_wrapper<const type>> &argument_types) const;

  private:
    std::vector<std::shared_ptr<component_type>> parents;   // the base types (i.e. the types this type inherits from)..
    std::vector<std::unique_ptr<constructor>> constructors; // the constructors of the type..
  };

  class predicate : public type, public scope
  {
  public:
    predicate(std::shared_ptr<scope> parent, const std::string &name);
    virtual ~predicate() = default;

  private:
    std::vector<std::shared_ptr<predicate>> parents; // the base predicates (i.e. the predicates this predicate inherits from)..
  };
} // namespace riddle
