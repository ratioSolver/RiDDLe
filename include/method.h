#pragma once

#include "scope.h"

namespace riddle
{
  class method : public scope
  {
  public:
    RIDDLE_EXPORT method(scope &scp, std::string name, std::vector<field_ptr> &args);
    virtual ~method() = default;

  private:
    std::string name;
  };

  using method_ptr = utils::u_ptr<method>;
} // namespace riddle
