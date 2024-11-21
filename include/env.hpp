#pragma once

#include <memory>
#include <map>
#include <string>
#include <string_view>

namespace riddle
{
  class core;
  class item;

  /**
   * @class env env.hpp "include/env.hpp"
   * @brief The environment class.
   *
   * The environment class is a container for items. It is used to store and
   * retrieve items by their name. Environments can be nested, with each
   * environment having a parent environment. When an item is retrieved, the
   * environment will first search for the item in its own items. If the item is
   * not found, the environment will search for the item in its parent
   * environment. This process continues until the item is found or there are no
   * more parent environments to search.
   */
  class env
  {
  public:
    env(core &c, env &parent, std::map<std::string, std::shared_ptr<item>, std::less<>> &&items = {}) noexcept;
    virtual ~env() = default;

    /**
     * @brief Retrieves an item by its name.
     *
     * This function retrieves an item by its name. The function will first
     * search for the item in the environment's items. If the item is not found,
     * the function will search for the item in the parent environment. This
     * process continues until the item is found or there are no more parent
     * environments to search.
     *
     * @param name The name of the item to retrieve.
     * @return The item with the given name, or nullptr if the item is not found.
     */
    [[nodiscard]] virtual std::shared_ptr<item> get(std::string_view name) noexcept;

  private:
    core &cr;
    env &parent;

  protected:
    std::map<std::string, std::shared_ptr<item>, std::less<>> items;
  };
} // namespace riddle
