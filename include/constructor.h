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
    RIDDLE_EXPORT constructor(complex_type &tp, std::vector<field_ptr> args, const std::vector<riddle::id_token> &ins, const std::vector<std::vector<ast::expression_ptr>> &ivs, const std::vector<ast::statement_ptr> &body);
    virtual ~constructor() = default;

    const std::vector<std::reference_wrapper<field>> &get_args() const { return args; }

    RIDDLE_EXPORT void call(expr &self, std::vector<expr> exprs);

  private:
    std::vector<std::reference_wrapper<field>> args;                // the arguments of this constructor..
    const std::vector<riddle::id_token> &init_names;                // the parameter names in the init-list..
    const std::vector<std::vector<ast::expression_ptr>> &init_vals; // for each parameter name in the init-list, its initializzation values..
    const std::vector<ast::statement_ptr> &body;                    // the body of the constructor..
  };
} // namespace riddle
