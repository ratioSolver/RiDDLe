#pragma once

#include "parser.h"
#include <string>

namespace riddle
{
  class type;

  class field
  {
  public:
    field(type &tp, const std::string &name, const ast::expression_ptr &e = ast::expression_ptr(), bool synthetic = false) : tp(tp), name(name), e(e), synthetic(synthetic) {}

    type &get_type() { return tp; }
    const type &get_type() const { return tp; }
    const std::string &get_name() const { return name; }
    const ast::expression_ptr &get_expression() const { return e; }
    bool is_synthetic() const { return synthetic; }

  private:
    type &tp;
    std::string name;
    const ast::expression_ptr &e;
    bool synthetic;
  };
} // namespace riddle
