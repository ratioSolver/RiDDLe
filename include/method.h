#pragma once

#include "scope.h"

namespace riddle
{
  class method : public scope
  {
  public:
    RIDDLE_EXPORT method(scope &scp);
    virtual ~method() = default;

  private:
    /* data */
  };
} // namespace riddle
