#pragma once

#include "item.hpp"
#include "lexer.hpp"

namespace riddle
{
  class scope;

  class expression
  {
  public:
    expression() = default;
    virtual ~expression() = default;

    virtual std::shared_ptr<item> evaluate(scope &scp, env &ctx) = 0;
  };

  class bool_expression final : public expression
  {
  public:
    bool_expression(const bool_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, env &ctx) override;

  private:
    bool_token l;
  };
} // namespace riddle
