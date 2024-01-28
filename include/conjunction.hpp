#pragma once

#include "scope.hpp"
#include "env.hpp"
#include "rational.hpp"

namespace riddle
{
  class conjunction : public scope
  {
  public:
    conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx, const utils::rational cst);

    /**
     * @brief Get the cost of the conjunction.
     *
     * @return utils::rational The cost of the conjunction.
     */
    utils::rational get_cost() const { return cst; }

  private:
    std::shared_ptr<env> ctx;
    const utils::rational cst;
  };
} // namespace riddle
