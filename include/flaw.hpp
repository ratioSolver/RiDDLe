#pragma once

#include "rational.hpp"
#include "json.hpp"
#include <vector>
#include <cstdint>
#include <memory>

namespace riddle
{
  class core;
  class resolver;

  /**
   * @class flaw
   * @brief Represents a flaw in the system, which can be resolved by associated resolvers.
   *
   * The flaw class models a flaw within the core system, maintaining its causes and possible resolvers.
   * It is designed to be used with shared pointers and supports polymorphic behavior.
   */
  class flaw
  {
  public:
    flaw(core &cr, std::vector<std::shared_ptr<resolver>> &&causes);
    flaw(const flaw &) = delete;
    virtual ~flaw() = default;

    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    [[nodiscard]] virtual utils::rational get_estimated_cost() const noexcept = 0;

    [[nodiscard]] core &get_core() const noexcept { return cr; }

    [[nodiscard]] const std::vector<std::shared_ptr<resolver>> &get_causes() const noexcept { return causes; }
    [[nodiscard]] std::vector<std::shared_ptr<resolver>> &get_resolvers() noexcept { return resolvers; }
    [[nodiscard]] const std::vector<std::shared_ptr<resolver>> &get_resolvers() const noexcept { return resolvers; }

    [[nodiscard]] virtual json::json to_json() const;

  protected:
    template <typename Tp, typename... Args>
    Tp &new_resolver(Args &&...args) noexcept
    {
      static_assert(std::is_base_of_v<resolver, Tp>, "Tp must be a subclass of resolver");
      auto r = std::make_shared<Tp>(std::forward<Args>(args)...);
      auto &r_ref = *r;
      resolvers.emplace_back(std::move(r));
      return r_ref;
    }

    void clear_resolvers() noexcept { resolvers.clear(); }

  protected:
    core &cr; // the core this flaw belongs to..

  private:
    std::vector<std::shared_ptr<resolver>> causes;    // the causes that led to this flaw..
    std::vector<std::shared_ptr<resolver>> resolvers; // the resolvers for this flaw..
  };

  /**
   * @brief Determines whether two flaws share any common ancestors.
   *
   * This function checks if the given flaws `a` and `b` have any ancestors in common
   * within the flaw hierarchy. It can be used to identify relationships or dependencies
   * between flaws based on their ancestry.
   *
   * @param a The first flaw to compare.
   * @param b The second flaw to compare.
   * @return true if `a` and `b` have at least one common ancestor; false otherwise.
   */
  [[nodiscard]] bool have_common_ancestors(const flaw &a, const flaw &b);
} // namespace riddle
