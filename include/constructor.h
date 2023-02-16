#pragma once

#include "scope.h"

namespace riddle
{
  class constructor : public scope
  {
  public:
    RIDDLE_EXPORT constructor(type &tp, std::vector<field_ptr> &args);
    virtual ~constructor() = default;

  private:
    /* data */
  };

  using constructor_ptr = utils::u_ptr<constructor>;
} // namespace riddle
