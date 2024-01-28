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

    virtual std::shared_ptr<item> evaluate(scope &scp, env &ctx) const = 0;

    virtual std::string to_string() const = 0;
  };

  class bool_expression final : public expression
  {
  public:
    bool_expression(const bool_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, env &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    bool_token l;
  };

  class int_expression final : public expression
  {
  public:
    int_expression(const int_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, env &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    int_token l;
  };

  class real_expression final : public expression
  {
  public:
    real_expression(const real_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, env &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    real_token l;
  };

  class string_expression final : public expression
  {
  public:
    string_expression(const string_token &l) : l(l) {}

    std::shared_ptr<item> evaluate(scope &scp, env &ctx) const override;

    std::string to_string() const override { return l.to_string(); }

  private:
    string_token l;
  };
} // namespace riddle
