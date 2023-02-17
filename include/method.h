#pragma once

#include "scope.h"
#include <functional>

namespace riddle
{
  class method : public scope
  {
  public:
    RIDDLE_EXPORT method(scope &scp, std::string name, std::vector<field_ptr> &args, std::vector<ast::statement_ptr> &body);
    virtual ~method() = default;

    std::vector<std::reference_wrapper<field>> &get_args() { return args; }

  private:
    std::string name;
    std::vector<std::reference_wrapper<field>> args;
    std::vector<ast::statement_ptr> body;
  };
} // namespace riddle
