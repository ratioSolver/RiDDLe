#pragma once

#include "scope.h"

namespace riddle
{
  class type : public scope
  {
  public:
    RIDDLE_EXPORT type(scope &scp, const std::string &name);
    virtual ~type() = default;

  private:
    std::string name;
  };

  using type_ptr = utils::u_ptr<type>;
} // namespace riddle
