#pragma once

#include <memory>
#include <vector>
#include <map>
#include <optional>
#include "field.hpp"

namespace riddle
{
  class core;
  class method;
  class predicate;
  class predicate_declaration;

  class scope : public std::enable_shared_from_this<scope>
  {
    friend class predicate_declaration;

  public:
    scope(core &c, std::shared_ptr<scope> parent = nullptr);
    virtual ~scope() = default;

    /**
     * @brief Get the core object.
     *
     * @return core& The core object.
     */
    [[nodiscard]] core &get_core() const { return c; }

    /**
     * @brief Get the enclosing scope.
     *
     * @return std::shared_ptr<scope> The enclosing scope.
     */
    [[nodiscard]] std::shared_ptr<scope> get_parent() const { return parent; }

    /**
     * @brief Get a field by name.
     *
     * @param name The name of the field.
     * @return std::optional<std::reference_wrapper<field>> The field.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<field>> get_field(const std::string &name) const noexcept;

    /**
     * @brief Get the fields in this scope.
     *
     * @return const std::map<std::string, std::unique_ptr<field>>& The fields.
     */
    [[nodiscard]] const std::map<std::string, std::unique_ptr<field>> &get_fields() const noexcept { return fields; }

    /**
     * @brief Get a method by name and argument types.
     *
     * @param name The name of the method.
     * @param argument_types The argument types.
     * @return std::optional<std::reference_wrapper<method>> The method.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<method>> get_method(const std::string &name, const std::vector<std::reference_wrapper<const type>> &argument_types) const { return parent->get_method(name, argument_types); }

    /**
     * @brief Get a type by name.
     *
     * @param name The name of the type.
     * @return std::optional<std::reference_wrapper<type>> The type.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<type>> get_type(const std::string &name) const { return parent->get_type(name); }

    /**
     * @brief Get a predicate by name.
     *
     * @param name The name of the predicate.
     * @return std::optional<std::reference_wrapper<predicate>> The predicate.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<predicate>> get_predicate(const std::string &name) const { return parent->get_predicate(name); }

  protected:
    void add_field(std::unique_ptr<field> &&field);

  private:
    core &c;
    std::shared_ptr<scope> parent;

  protected:
    std::map<std::string, std::unique_ptr<field>> fields;
  };
} // namespace riddle
