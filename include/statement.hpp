#pragma once

#include "expression.hpp"

namespace riddle
{
  class statement
  {
  public:
    virtual ~statement() = default;

    virtual void execute(const scope &scp, env &ctx) const = 0;
  };
} // namespace riddle
