#pragma once

#include "expression.hpp"
#include <stdexcept>

namespace riddle
{
  class inconsistency_exception : public std::exception
  {
  public:
    inconsistency_exception() = default;

    virtual const char *what() const noexcept override { return "inconsistency detected"; }
  };

  class statement
  {
  public:
    virtual ~statement() = default;

    virtual void execute(const scope &scp, env &ctx) const = 0;
  };

  class local_field_statement final : public statement
  {
  public:
    local_field_statement(std::vector<id_token> &&field_type, std::vector<std::pair<id_token, std::unique_ptr<expression>>> &&fields) : field_type(std::move(field_type)), fields(std::move(fields)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> field_type;
    std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;
  };

  class assignment_statement final : public statement
  {
  public:
    assignment_statement(std::vector<id_token> &&object_id, id_token &&field_id, std::unique_ptr<expression> value) : object_id(std::move(object_id)), field_id(std::move(field_id)), value(std::move(value)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> object_id;
    id_token field_id;
    std::unique_ptr<expression> value;
  };

  class expression_statement final : public statement
  {
  public:
    expression_statement(std::unique_ptr<expression> xpr) : xpr(std::move(xpr)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> xpr;
  };

  class conjunction_statement final : public statement
  {
  public:
    conjunction_statement(std::vector<std::unique_ptr<statement>> &&stmts, std::unique_ptr<expression> cst = nullptr) : stmts(std::move(stmts)), cst(std::move(cst)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<statement>> stmts;
    std::unique_ptr<expression> cst;
  };

  class disjunction_statement final : public statement
  {
  public:
    disjunction_statement(std::vector<std::unique_ptr<conjunction_statement>> &&blocks) : blocks(std::move(blocks)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<std::unique_ptr<conjunction_statement>> blocks;
  };

  class for_all_statement final : public statement
  {
  public:
    for_all_statement(std::vector<id_token> &&enum_type, id_token &&enum_id, std::vector<std::unique_ptr<statement>> stmts) : enum_type(std::move(enum_type)), enum_id(std::move(enum_id)), stmts(std::move(stmts)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::vector<id_token> enum_type;
    id_token enum_id;
    std::vector<std::unique_ptr<statement>> stmts;
  };

  class return_statement final : public statement
  {
  public:
    return_statement(std::unique_ptr<expression> xpr) : xpr(std::move(xpr)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    std::unique_ptr<expression> xpr;
  };

  class formula_statement final : public statement
  {
  public:
    formula_statement(bool is_fact, id_token &&id, std::vector<id_token> &&tau, id_token &&predicate_name, std::vector<std::pair<id_token, std::unique_ptr<expression>>> &&args) : is_fact(is_fact), id(std::move(id)), tau(std::move(tau)), predicate_name(std::move(predicate_name)), args(std::move(args)) {}

    void execute(const scope &scp, env &ctx) const override;

  private:
    bool is_fact;
    id_token id;
    std::vector<id_token> tau;
    id_token predicate_name;
    std::vector<std::pair<id_token, std::unique_ptr<expression>>> args;
  };
} // namespace riddle
