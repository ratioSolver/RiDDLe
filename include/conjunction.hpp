#pragma once

#include "scope.hpp"
#include "env.hpp"

namespace riddle
{
  class conjunction final : public scope
  {
  public:
    conjunction(core &c, scope &parent, env &&ctx) noexcept;

  private:
    env ctx;
  };
} // namespace riddle
