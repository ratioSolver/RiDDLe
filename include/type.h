#pragma once

#include "scope.h"

namespace riddle
{
  class type : public scope
  {
  public:
    RIDDLE_EXPORT type(scope &scp);
    virtual ~type() = default;

  private:
    /* data */
  };
} // namespace riddle
