#pragma once

#include <memory>
#include <map>
#include <optional>
#include "field.hpp"

namespace riddle
{
  class core;

  class scope
  {
  public:
    scope(core &c, std::shared_ptr<scope> parent = nullptr);
    virtual ~scope() = default;

    /**
     * @brief Get the core object.
     *
     * @return core& The core object.
     */
    core &get_core() const { return c; }

    /**
     * @brief Get the enclosing scope.
     *
     * @return std::shared_ptr<scope> The enclosing scope.
     */
    std::shared_ptr<scope> get_parent() const { return parent; }

    /**
     * @brief Get a field by name.
     *
     * @param name The name of the field.
     * @return std::optional<field> The field.
     */
    virtual std::optional<field> get_field(const std::string &name) const;

  private:
    core &c;
    std::shared_ptr<scope> parent;

  protected:
    std::map<std::string, std::unique_ptr<field>> fields;
  };
} // namespace riddle
