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
     *
     */
    void execute();

  private:
    std::shared_ptr<env> ctx;
    const conjunction_statement &conj;
  };
} // namespace riddle
