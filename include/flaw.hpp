#pragma once

#include "rational.hpp"
#include "json.hpp"
#include "core.hpp"
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
    friend class core;

  public:
    flaw(core &cr, std::optional<std::reference_wrapper<resolver>> cause) : flaw(cr, cause.has_value() ? std::vector<std::reference_wrapper<resolver>>{cause.value()} : std::vector<std::reference_wrapper<resolver>>{}) {}
    flaw(core &cr, std::vector<std::reference_wrapper<resolver>> &&causes);
    flaw(const flaw &) = delete;
    virtual ~flaw() = default;

    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    [[nodiscard]] core &get_core() const noexcept { return cr; }
    [[nodiscard]] const utils::rational &get_estimated_cost() const noexcept { return est_cost; }

    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_causes() const noexcept { return causes; }
    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_supports() const noexcept { return supports; }
    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_resolvers() const noexcept { return resolvers; }

    [[nodiscard]] virtual json::json to_json() const;

  protected:
    template <typename Tp, typename... Args>
    Tp &new_resolver(Args &&...args) noexcept { return cr.new_resolver<Tp>(std::forward<Args>(args)...); }

    void add_support(resolver &res) noexcept;

  private:
    virtual void compute_resolvers() = 0;
    friend bool have_common_ancestors(const flaw &a, const flaw &b);

  protected:
    core &cr; // the core this flaw belongs to..

  private:
    utils::rational est_cost = utils::rational::positive_infinite; // the estimated cost of this flaw..
    std::vector<std::reference_wrapper<resolver>> causes;          // the causes that led to this flaw..
    std::vector<std::reference_wrapper<resolver>> supports;        // the resolvers supported by this flaw..
    std::vector<std::reference_wrapper<resolver>> resolvers;       // the resolvers for this flaw..
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
