#pragma once

#include "scope.h"
#include <functional>

namespace riddle
{
  class complex_type;

  class constructor : public scope
  {
    friend class ast::constructor_expression;

  public:
    RIDDLE_EXPORT constructor(complex_type &tp, std::vector<field_ptr> &args, std::vector<ast::statement_ptr> &body);
    virtual ~constructor() = default;

    const std::vector<std::reference_wrapper<field>> &get_args() const { return args; }

    RIDDLE_EXPORT void call(expr &self, std::vector<expr> exprs);

  private:
    std::vector<std::reference_wrapper<field>> args;
    std::vector<ast::statement_ptr> body;
  };
} // namespace riddle
