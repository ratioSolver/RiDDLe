#pragma once

#include "statement.hpp"

namespace riddle
{
  class conjunction final
  {
  public:
    conjunction(const scope &scp, env &&ctx, const utils::rational cst = utils::rational::one, const std::vector<std::unique_ptr<statement>> &body = {}) noexcept;

    /**
     * @brief Executes the conjunction operation.
     *
     * This function performs the necessary actions to execute the conjunction.
     * It does not take any parameters and does not return any value.
     */
    void execute();

  private:
    const scope &scp;                                    // the scope in which the conjunction is evaluated
    env ctx;                                             // the environment context
    utils::rational cst;                                 // the cost of the conjunction
    const std::vector<std::unique_ptr<statement>> &body; // the body of the conjunction
  };
} // namespace riddle
