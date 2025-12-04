#pragma once

#include "json.hpp"
#include <map>
#include <string>
#include <string_view>
#include <memory>

namespace riddle
{
  class core;
  class term;
  using expr = std::shared_ptr<term>;
  using const_expr = std::shared_ptr<const term>;
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

    /**
     * @brief Retrieves an item by its name and casts it to a specific type.
     *
     * This function retrieves an item by its name and attempts to cast it to
     * the specified type. If the item is not found or cannot be cast to the
     * specified type, an exception is thrown.
     *
     * @tparam T The type to cast the item to.
     * @param name The name of the item to retrieve.
     * @return A shared pointer to the item casted to type T.
     * @throws std::out_of_range if the item is not found.
     * @throws std::bad_cast if the item cannot be cast to type T.
     */
    template <typename T>
    [[nodiscard]] std::shared_ptr<T> get(std::string_view name)
    {
      if (auto item = get(name); item)
      {
        if (auto term_ptr = std::dynamic_pointer_cast<T>(item))
          return term_ptr;
        else
          throw std::bad_cast();
      }
      else
        throw std::out_of_range("Item not found: " + std::string(name));
    }

    /**
     * @brief Retrieves the items in the environment.
     *
     * This function returns a reference to the map containing the items in the
     * environment.
     *
     * @return A reference to the map of items.
     */
    [[nodiscard]] const std::map<std::string, expr, std::less<>> &get_items() const noexcept { return items; }

    /**
     * @brief Converts the environment to a JSON object.
     *
     * This function converts the environment's items to a JSON object. Each
     * item is represented by its name and value. The value is converted to JSON
     * using the `to_json` method of the item.
     *
     * @return A JSON object representing the environment's items.
     */
    [[nodiscard]] virtual json::json to_json() const;

  private:
    core &cr;
    env &parent;

  protected:
    std::map<std::string, expr, std::less<>> items;
  };
} // namespace riddle
