#pragma once

#include <memory>
#include <map>
#include <string>

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

  /**
   * @brief The environment class.
   *
   * An environment is a collection of items.
   */
  class env : public std::enable_shared_from_this<env>
  {
    friend class constructor;
    friend class method;
    friend class local_field_statement;
    friend class assignment_statement;
    friend class for_all_statement;
    friend class return_statement;

  public:
    env(core &c, std::shared_ptr<env> parent = nullptr);
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
    [[nodiscard]] virtual std::shared_ptr<item> get(const std::string &name) const noexcept;

  private:
    core &cr;
    std::shared_ptr<env> parent;

  protected:
    std::map<std::string, std::shared_ptr<item>> items;
  };
} // namespace riddle
