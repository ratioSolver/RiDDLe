#pragma once

#include "scope.hpp"
#include "env.hpp"

namespace riddle
{
  class conjunction : public scope
  {
  public:
    conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx);

  private:
    std::shared_ptr<env> ctx;
  };
} // namespace riddle
