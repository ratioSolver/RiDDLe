#pragma once

#include "scope.h"

namespace riddle
{
  class constructor : public scope
  {
  public:
    RIDDLE_EXPORT constructor(type &tp);
    virtual ~constructor() = default;

  private:
    /* data */
  };
} // namespace riddle
