#pragma once

#include "rational.hpp"
#include "json.hpp"
#include <vector>
#include <memory>

namespace riddle
{
  class flaw;

  /**
   * @class resolver
   * @brief Represents a resolver that can address a specific flaw.
   *
   * The resolver class models a mechanism to resolve a flaw within the core system.
   * It maintains its associated flaw, intrinsic cost, and preconditions.
   * It is designed to be used with shared pointers and supports polymorphic behavior.
   */
  class resolver
  {
    friend class flaw;

  public:
    resolver(flaw &flw, utils::rational &&intrinsic_cost);
    resolver(const resolver &) = delete;
    virtual ~resolver() = default;

    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    [[nodiscard]] flaw &get_flaw() const noexcept { return flw; }

    [[nodiscard]] const utils::rational &get_intrinsic_cost() const noexcept { return intrinsic_cost; }
    [[nodiscard]] const std::vector<std::shared_ptr<flaw>> &get_preconditions() const noexcept { return preconditions; }

    [[nodiscard]] utils::rational get_estimated_cost() const noexcept;

    [[nodiscard]] virtual json::json to_json() const;

  protected:
    flaw &flw; // the flaw solved by this resolver..

  private:
    const utils::rational intrinsic_cost;             // the intrinsic cost of this resolver..
    std::vector<std::shared_ptr<flaw>> preconditions; // the preconditions of this resolver..
  };
} // namespace riddle
