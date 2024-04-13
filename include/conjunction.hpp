#pragma once

#include "scope.hpp"
#include "env.hpp"
#include "rational.hpp"

namespace riddle
{
  class conjunction_statement;

  class conjunction : public scope
  {
  public:
    conjunction(scope &parent, std::shared_ptr<env> ctx, const conjunction_statement &conj);

    /**
     * @brief Execute the conjunction.
     */
    void execute();

    /**
     * @brief Compute the cost of the conjunction.
     *
     * @return The cost of the conjunction.
     */
    [[nodiscard]] std::shared_ptr<arith_item> compute_cost() const noexcept;

  private:
    std::shared_ptr<env> ctx;
    const conjunction_statement &conj;
  };
} // namespace riddle
