#pragma once

#include "scope.h"
#include <functional>

namespace riddle
{
  class method : public scope
  {
  public:
    RIDDLE_EXPORT method(scope &scp, std::string name, std::vector<field_ptr> &&args, const std::vector<ast::statement_ptr> &body, type *ret_type = nullptr);
    virtual ~method() = default;

    type *get_return_type() { return ret_type; }
    const std::string &get_name() const { return name; }
    std::vector<std::reference_wrapper<field>> &get_args() { return args; }

    RIDDLE_EXPORT expr call(expr &self, std::vector<expr> exprs);

  private:
    type *ret_type;
    std::string name;
    std::vector<std::reference_wrapper<field>> args;
    const std::vector<ast::statement_ptr> &body;
  };
} // namespace riddle
