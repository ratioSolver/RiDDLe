#pragma once

#include "scope.hpp"
#include "env.hpp"
#include "rational.hpp"

namespace riddle
{
  class statement;

  class conjunction : public scope
  {
  public:
    conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx, const utils::rational &cst, std::vector<std::unique_ptr<statement>> &&body);

    /**
     * @brief Get the cost of the conjunction.
     *
     * @return utils::rational The cost of the conjunction.
     */
    utils::rational get_cost() const { return cst; }

    /**
     * @brief Execute the conjunction.
     *
     */
    void execute();

  private:
    std::shared_ptr<env> ctx;
    const utils::rational cst;
    const std::vector<std::unique_ptr<statement>> body;
  };
} // namespace riddle
