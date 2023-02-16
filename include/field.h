#pragma once

#include "parser.h"
#include <string>

namespace riddle
{
  class type;

  namespace ast
  {
    class expression;
  } // namespace ast

  class field
  {
  public:
    field(type &tp, const std::string &name, utils::u_ptr<ast::expression> e) : tp(tp), name(name), e(std::move(e)) {}

    type &get_type() { return tp; }
    const std::string &get_name() const { return name; }

  private:
    type &tp;
    std::string name;
    utils::u_ptr<ast::expression> e;
  };

  using field_ptr = utils::u_ptr<field>;
} // namespace riddle
