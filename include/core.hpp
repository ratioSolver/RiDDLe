#pragma once

#include "scope.hpp"
#include "env.hpp"

namespace riddle
{
  class core : public scope, public env
  {
  public:
    core();
    virtual ~core() = default;

    /**
     * @brief Get a field by name.
     *
     * @param name The name of the field.
     * @return std::optional<field> The field.
     */
    virtual std::optional<field> get_field(const std::string &name) const override;

    /**
     * @brief Get an item by name.
     *
     * @param name The name of the item.
     * @return std::shared_ptr<item> The item.
     */
    virtual std::shared_ptr<item> get(const std::string &name) const override;
  };
} // namespace riddle
