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

  using method_ptr = utils::u_ptr<method>;
} // namespace riddle
