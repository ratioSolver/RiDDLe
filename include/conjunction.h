#pragma once

#include "scope.h"
#include "rational.h"

namespace riddle
{
  class conjunction : public scope
  {
  public:
    conjunction(scope &scp, env ctx, const utils::rational cst, const std::vector<ast::statement_ptr> &body);

    const utils::rational &get_cst() const { return cst; }

    void execute();

  private:
    env ctx;
    const utils::rational cst;
    const std::vector<ast::statement_ptr> &body;
  };
} // namespace riddle
