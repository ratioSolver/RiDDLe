#pragma once

#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <optional>

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
    env(core &c, env &parent, std::map<std::string, std::unique_ptr<item>, std::less<>> &&items = {}) noexcept;
    virtual ~env() = default;

    /**
     * @brief Retrieves an item by its name.
     *
     * This function searches for an item with the specified name and returns it
     * wrapped in a std::optional. If the item is found, it is returned as a
     * std::reference_wrapper. If the item is not found, an empty std::optional
     * is returned.
     *
     * @param name The name of the item to retrieve.
     * @return std::optional<std::reference_wrapper<item>> The item wrapped in
     * a std::optional if found, otherwise an empty std::optional.
     */
    [[nodiscard]] virtual std::optional<std::reference_wrapper<item>> get(std::string_view name) noexcept;

  private:
    core &cr;
    env &parent;

  protected:
    std::map<std::string, std::unique_ptr<item>, std::less<>> items;
  };
} // namespace riddle
