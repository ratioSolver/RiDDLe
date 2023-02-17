#pragma once

#include "parser.h"
#include <string>

namespace riddle
{
  class type;

  class field
  {
  public:
    field(type &tp, const std::string &name, ast::expression_ptr e = nullptr, bool synthetic = false) : tp(tp), name(name), e(std::move(e)), synthetic(synthetic) {}

    type &get_type() { return tp; }
    const std::string &get_name() const { return name; }
    ast::expression_ptr &get_expression() { return e; }
    bool is_synthetic() const { return synthetic; }

  private:
    type &tp;
    std::string name;
    ast::expression_ptr e;
    bool synthetic;
  };
} // namespace riddle
