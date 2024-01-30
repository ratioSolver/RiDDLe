#pragma once

#include "expression.hpp"

namespace riddle
{
  class statement
  {
  public:
    statement() = default;
    virtual ~statement() = default;

    virtual void execute(scope &scp, env &ctx) const = 0;

    virtual std::string to_string() const = 0;
  };

  class local_field_statement final : public statement
  {
  public:
    local_field_statement(std::vector<id_token> &&field_type, std::vector<id_token> &&fields, std::vector<std::unique_ptr<expression>> &&initializers) : field_type(std::move(field_type)), fields(std::move(fields)), initializers(std::move(initializers)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      std::string result = field_type[0].to_string();
      for (size_t i = 1; i < field_type.size(); ++i)
        result += "." + field_type[i].to_string();
      result += " " + fields[0].to_string();
      if (initializers[0])
        result += " = " + initializers[0]->to_string();
      for (size_t i = 1; i < fields.size(); ++i)
      {
        result += ", " + fields[i].to_string();
        if (initializers[i])
          result += " = " + initializers[i]->to_string();
      }
      return result + ";";
    }

  private:
    std::vector<id_token> field_type;
    std::vector<id_token> fields;
    std::vector<std::unique_ptr<expression>> initializers;
  };

  class assignment_statement final : public statement
  {
  public:
    assignment_statement(std::vector<id_token> &&lhs, id_token &&object_id, std::unique_ptr<expression> &&rhs) : lhs(std::move(lhs)), object_id(std::move(object_id)), rhs(std::move(rhs)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      if (lhs.empty())
        return object_id.to_string() + " = " + rhs->to_string() + ";";
      std::string result = lhs[0].to_string();
      for (size_t i = 1; i < lhs.size(); ++i)
        result += "." + lhs[i].to_string();
      return result + object_id.to_string() + " = " + rhs->to_string() + ";";
    }

  private:
    std::vector<id_token> lhs;
    id_token object_id;
    std::unique_ptr<expression> rhs;
  };

  class expression_statement final : public statement
  {
  public:
    expression_statement(std::unique_ptr<expression> &&expr) : expr(std::move(expr)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override { return expr->to_string() + ";"; }

  private:
    std::unique_ptr<expression> expr;
  };

  class conjunction_statement final : public statement
  {
  public:
    conjunction_statement(std::vector<std::unique_ptr<statement>> &&statements, std::unique_ptr<expression> &&cst = nullptr) : statements(std::move(statements)), cst(std::move(cst)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const
    {
      std::string result = "{\n";
      for (const auto &stmt : statements)
        result += "  " + stmt->to_string() + "\n";
      result += "}";
      if (cst)
        result += " [" + cst->to_string() + "]";
      return result;
    }

  private:
    std::vector<std::unique_ptr<statement>> statements;
    std::unique_ptr<expression> cst;
  };

  class disjunction_statement final : public statement
  {
  public:
    disjunction_statement(std::vector<conjunction_statement> &&blocks) : blocks(std::move(blocks)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      std::string result = blocks[0].to_string();
      for (size_t i = 1; i < blocks.size(); ++i)
        result += " or " + blocks[i].to_string();
      return result;
    }

  private:
    std::vector<conjunction_statement> blocks;
  };

  class for_all_statement final : public statement
  {
  public:
    for_all_statement(std::vector<id_token> &&enum_type, id_token &&enum_id, std::vector<std::unique_ptr<statement>> &&statements) : enum_type(std::move(enum_type)), enum_id(std::move(enum_id)), statements(std::move(statements)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      std::string result = "for (" + enum_type[0].to_string();
      for (size_t i = 1; i < enum_type.size(); ++i)
        result += "." + enum_type[i].to_string();
      result += " " + enum_id.to_string() + ") {\n";
      for (const auto &stmt : statements)
        result += "  " + stmt->to_string() + "\n";
      return result + "}";
    }

  private:
    std::vector<id_token> enum_type;
    id_token enum_id;
    std::vector<std::unique_ptr<statement>> statements;
  };

  class formula_argument final
  {
  public:
    formula_argument(id_token &&object_id, std::unique_ptr<expression> &&expr) : object_id(std::move(object_id)), expr(std::move(expr)) {}

    void execute(scope &scp, env &ctx) const;

    std::string to_string() const
    {
      if (expr)
        return object_id.to_string() + " = " + expr->to_string();
      return object_id.to_string();
    }

  private:
    id_token object_id;
    std::unique_ptr<expression> expr;
  };

  class return_statement final : public statement
  {
  public:
    return_statement(std::unique_ptr<expression> &&expr) : expr(std::move(expr)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      if (expr)
        return "return " + expr->to_string() + ";";
      return "return;";
    }

  private:
    std::unique_ptr<expression> expr;
  };

  class formula_statement final : public statement
  {
  public:
    formula_statement(bool is_fact, id_token &&formula_name, std::vector<id_token> &&formula_scope, id_token &&predicate_name, std::vector<formula_argument> &&arguments) : is_fact(is_fact), formula_name(std::move(formula_name)), formula_scope(std::move(formula_scope)), predicate_name(std::move(predicate_name)), arguments(std::move(arguments)) {}

    void execute(scope &scp, env &ctx) const override;

    std::string to_string() const override
    {
      std::string result = is_fact ? "fact " : "goal ";
      result += formula_name.to_string() + " = new ";
      if (!formula_scope.empty())
      {
        result += formula_scope[0].to_string();
        for (size_t i = 1; i < formula_scope.size(); ++i)
          result += "." + formula_scope[i].to_string();
        result += ".";
      }
      result += predicate_name.to_string() + "(";
      if (!arguments.empty())
      {
        result += arguments[0].to_string();
        for (size_t i = 1; i < arguments.size(); ++i)
          result += ", " + arguments[i].to_string();
      }
      return result + ");";
    }

  private:
    bool is_fact;
    id_token formula_name;
    std::vector<id_token> formula_scope;
    id_token predicate_name;
    std::vector<formula_argument> arguments;
  };
} // namespace riddle
