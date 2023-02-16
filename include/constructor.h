#pragma once

#include "scope.h"
#include <functional>

namespace riddle
{
  class constructor : public scope
  {
  public:
    RIDDLE_EXPORT constructor(type &tp, std::vector<field_ptr> &args, std::vector<ast::statement_ptr> &body);
    virtual ~constructor() = default;

  private:
    std::vector<std::reference_wrapper<field>> args;
    std::vector<ast::statement_ptr> body;
  };

  using constructor_ptr = utils::u_ptr<constructor>;
} // namespace riddle
