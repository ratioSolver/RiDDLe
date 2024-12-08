#pragma once

#include "expression.hpp"

namespace riddle
{
  class statement
  {
  public:
    virtual ~statement() = default;

    virtual void execute(const scope &scp, env &ctx) const = 0;
  };

  class local_field_statement final : public statement
  {
  public:
    local_field_statement(std::vector<id_token> &&field_type, std::vector<id_token> &&names, std::vector<std::unique_ptr<expression>> &&values) : field_type(std::move(field_type)), names(std::move(names)), values(std::move(values)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> field_type;
    std::vector<id_token> names;
    std::vector<std::unique_ptr<expression>> values;
  };
} // namespace riddle
