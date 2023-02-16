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
    field(type &tp, const std::string &name, utils::u_ptr<ast::expression> e = nullptr, bool synthetic = false) : tp(tp), name(name), e(std::move(e)), synthetic(synthetic) {}

    type &get_type() { return tp; }
    const std::string &get_name() const { return name; }
    utils::u_ptr<ast::expression> &get_expression() { return e; }
    bool is_synthetic() const { return synthetic; }

  private:
    type &tp;
    std::string name;
    utils::u_ptr<ast::expression> e;
    bool synthetic;
  };

  using field_ptr = utils::u_ptr<field>;
} // namespace riddle
