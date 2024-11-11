#pragma once

#include <memory>
#include <map>
#include <string>
#include <string_view>

namespace riddle
{
  class core;
  class item;
  class constructor;
  class method;
  class local_field_statement;
  class assignment_statement;
  class for_all_statement;
  class return_statement;
  class formula_statement;

  /**
   * @brief The environment class.
   *
   * An environment is a collection of items.
   */
  class env
  {
    friend class core;
    friend class constructor;
    friend class method;
    friend class local_field_statement;
    friend class assignment_statement;
    friend class for_all_statement;
    friend class return_statement;
    friend class formula_statement;

  public:
    env(core &c, std::shared_ptr<env> parent = nullptr, std::map<std::string, std::shared_ptr<item>, std::less<>> &&items = {});
    virtual ~env() = default;

    /**
     * @brief Get the core object.
     *
     * @return core& The core object.
     */
    [[nodiscard]] core &get_core() const { return cr; }
    /**
     * @brief Get the enclosing environment.
     *
     * @return std::shared_ptr<env> The enclosing environment.
     */
    [[nodiscard]] std::shared_ptr<env> get_parent() const { return parent; }

    /**
     * @brief Get an item by name.
     *
     * @param name The name of the item.
     * @return std::shared_ptr<item> The item.
     */
    [[nodiscard]] virtual std::shared_ptr<item> get(std::string_view name);

    /**
     * @brief Get the items of the environment.
     *
     * @return const std::map<std::string, std::shared_ptr<item>>& The items of the environment.
     */
    const std::map<std::string, std::shared_ptr<item>, std::less<>> &get_items() const { return items; }

  private:
    core &cr;
    std::shared_ptr<env> parent;

  protected:
    std::map<std::string, std::shared_ptr<item>, std::less<>> items;
  };
} // namespace riddle
