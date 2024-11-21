#pragma once

#include "item.hpp"

namespace riddle
{
  class expression
  {
  public:
    expression() = default;
    virtual ~expression() = default;

    [[nodiscard]] virtual std::shared_ptr<item> evaluate(env &e) const = 0;
  };
} // namespace riddle
