#pragma once

#include "json.hpp"
#include "memory.hpp"
#include <map>
#include <string>
#include <string_view>

namespace riddle
{
  class core;
  class term;
  using expr = utils::s_ptr<term>;
  class constructor;
  class method;
  class component_type;
  class local_field_statement;
  class assignment_statement;
  class for_all_statement;
  class return_statement;
  class formula_statement;
  class disjunction_statement;

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
    friend class constructor;
    friend class method;
    friend class component_type;
    friend class local_field_statement;
    friend class assignment_statement;
    friend class for_all_statement;
    friend class return_statement;
    friend class formula_statement;
    friend class disjunction_statement;
#ifdef COMPUTE_NAMES
    friend class core;
#endif

  public:
    env(core &c, env &parent, std::map<std::string, expr, std::less<>> &&items = {}) noexcept;
    env(const env &) = delete;
    env(env &&) = default;
    virtual ~env() = default;

    /**
     * @brief Retrieves the core object.
     *
     * This function returns a reference to the core object.
     *
     * @return A reference to the core object.
     */
    [[nodiscard]] core &get_core() const noexcept { return cr; }

    /**
     * @brief Retrieves the parent environment.
     *
     * This function returns a reference to the parent environment.
     *
     * @return A reference to the parent environment.
     */
    [[nodiscard]] env &get_parent() const noexcept { return parent; }

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
     * @return The item with the given name.
     * @throws std::out_of_range if the item is not found in the current or parent environment.
     */
    [[nodiscard]] virtual expr get(std::string_view name);

    [[nodiscard]] virtual json::json to_json() const;

  private:
    core &cr;
    env &parent;

  protected:
    std::map<std::string, expr, std::less<>> items;
  };
} // namespace riddle
