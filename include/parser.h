#pragma once

#include "lexer.h"
#include <vector>

namespace riddle
{
  class scope;
  class env;
  using env_ptr = utils::c_ptr<env>;
  class item;
  using expr = utils::c_ptr<item>;
  class field;
  using field_ptr = utils::u_ptr<field>;
  class type;
  using type_ptr = utils::u_ptr<type>;
  class method;
  using method_ptr = utils::u_ptr<method>;
  class constructor;
  using constructor_ptr = utils::u_ptr<constructor>;
  class predicate;
  using predicate_ptr = utils::u_ptr<predicate>;
  class conjunction;
  using conjunction_ptr = utils::u_ptr<conjunction>;

  class inconsistency_exception : public std::exception
  {
  public:
    inconsistency_exception() = default;

    virtual const char *what() const noexcept override { return "inconsistency detected"; }
  };

  namespace ast
  {
    class expression
    {
    public:
      expression() = default;
      expression(const expression &orig) = delete;
      virtual ~expression() = default;

      virtual expr evaluate(scope &scp, env &ctx) const = 0;

      virtual std::string to_string() const = 0;
    };
    using expression_ptr = utils::u_ptr<const expression>;

    class bool_literal_expression final : public expression
    {
    public:
      bool_literal_expression(const bool_token &l) : literal(l) {}
      bool_literal_expression(const bool_literal_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return literal.to_string(); }

    protected:
      const bool_token literal;
    };

    class int_literal_expression final : public expression
    {
    public:
      int_literal_expression(const int_token &l) : literal(l) {}
      int_literal_expression(const int_literal_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return literal.to_string(); }

    protected:
      const int_token literal;
    };

    class real_literal_expression final : public expression
    {
    public:
      real_literal_expression(const real_token &l) : literal(l) {}
      real_literal_expression(const real_literal_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return literal.to_string(); }

    protected:
      const real_token literal;
    };

    class string_literal_expression final : public expression
    {
    public:
      string_literal_expression(const string_token &l) : literal(l) {}
      string_literal_expression(const string_literal_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return literal.to_string(); }

    protected:
      const string_token literal;
    };

    class cast_expression final : public expression
    {
    public:
      cast_expression(std::vector<id_token> tp, expression_ptr e) : cast_to_type(std::move(tp)), xpr(std::move(e)) {}
      cast_expression(const cast_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = "(" + cast_to_type[0].to_string();
        for (size_t i = 1; i < cast_to_type.size(); ++i)
          result += "." + cast_to_type[i].to_string();
        result += ") " + xpr->to_string();
        return result;
      }

    protected:
      const std::vector<id_token> cast_to_type;
      const expression_ptr xpr;
    };

    class plus_expression final : public expression
    {
    public:
      plus_expression(expression_ptr e) : xpr(std::move(e)) {}
      plus_expression(const plus_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return "+" + xpr->to_string(); }

    protected:
      const expression_ptr xpr;
    };

    class minus_expression final : public expression
    {
    public:
      minus_expression(expression_ptr e) : xpr(std::move(e)) {}
      minus_expression(const minus_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return "-" + xpr->to_string(); }

    protected:
      const expression_ptr xpr;
    };

    class not_expression final : public expression
    {
    public:
      not_expression(expression_ptr e) : xpr(std::move(e)) {}
      not_expression(const not_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return "!" + xpr->to_string(); }

    protected:
      const expression_ptr xpr;
    };

    class constructor_expression final : public expression
    {
    public:
      constructor_expression(std::vector<id_token> it, std::vector<expression_ptr> es) : instance_type(std::move(it)), expressions(std::move(es)) {}
      constructor_expression(const constructor_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = "new " + instance_type[0].to_string();
        for (size_t i = 1; i < instance_type.size(); ++i)
          result += "." + instance_type[i].to_string();
        result += "(";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
          if (i > 0)
            result += ", ";
          result += expressions[i]->to_string();
        }
        result += ")";
        return result;
      }

    protected:
      const std::vector<id_token> instance_type;
      const std::vector<expression_ptr> expressions;
    };

    class eq_expression final : public expression
    {
    public:
      eq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      eq_expression(const eq_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " == " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class neq_expression final : public expression
    {
    public:
      neq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      neq_expression(const neq_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " != " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class lt_expression final : public expression
    {
    public:
      lt_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      lt_expression(const lt_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " < " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class leq_expression final : public expression
    {
    public:
      leq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      leq_expression(const leq_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " <= " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class geq_expression final : public expression
    {
    public:
      geq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      geq_expression(const geq_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " >= " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class gt_expression final : public expression
    {
    public:
      gt_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      gt_expression(const gt_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " > " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class function_expression final : public expression
    {
    public:
      function_expression(std::vector<id_token> is, const id_token &fn, std::vector<expression_ptr> es) : ids(std::move(is)), function_name(std::move(fn)), expressions(std::move(es)) {}
      function_expression(const function_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = ids[0].to_string();
        for (size_t i = 1; i < ids.size(); ++i)
          result += "." + ids[i].to_string();
        result += "." + function_name.to_string() + "(";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
          if (i > 0)
            result += ", ";
          result += expressions[i]->to_string();
        }
        result += ")";
        return result;
      }

    protected:
      const std::vector<id_token> ids;
      const id_token function_name;
      const std::vector<expression_ptr> expressions;
    };

    class id_expression final : public expression
    {
    public:
      id_expression(std::vector<id_token> is) : ids(std::move(is)) {}
      id_expression(const id_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = ids[0].to_string();
        for (size_t i = 1; i < ids.size(); ++i)
          result += "." + ids[i].to_string();
        return result;
      }

    protected:
      const std::vector<id_token> ids;
    };

    class implication_expression final : public expression
    {
    public:
      implication_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      implication_expression(const implication_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override { return left->to_string() + " -> " + right->to_string(); }

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class disjunction_expression final : public expression
    {
    public:
      disjunction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      disjunction_expression(const disjunction_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " | " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class conjunction_expression final : public expression
    {
    public:
      conjunction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      conjunction_expression(const conjunction_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " & " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class exct_one_expression final : public expression
    {
    public:
      exct_one_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      exct_one_expression(const exct_one_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " ^ " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class addition_expression final : public expression
    {
    public:
      addition_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      addition_expression(const addition_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " + " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class subtraction_expression final : public expression
    {
    public:
      subtraction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      subtraction_expression(const subtraction_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " - " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class multiplication_expression final : public expression
    {
    public:
      multiplication_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      multiplication_expression(const multiplication_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " * " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class division_expression final : public expression
    {
    public:
      division_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      division_expression(const division_expression &orig) = delete;

      RIDDLE_EXPORT expr evaluate(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = expressions[0]->to_string();
        for (size_t i = 1; i < expressions.size(); ++i)
          result += " / " + expressions[i]->to_string();
        return result;
      }

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class statement
    {
    public:
      statement() = default;
      statement(const statement &orig) = delete;
      virtual ~statement() = default;

      virtual void execute(scope &scp, env &ctx) const = 0;

      virtual std::string to_string() const = 0;
    };
    using statement_ptr = utils::u_ptr<const statement>;

    class local_field_statement final : public statement
    {
    public:
      local_field_statement(std::vector<id_token> ft, std::vector<id_token> ns, std::vector<expression_ptr> es) : field_type(std::move(ft)), names(std::move(ns)), xprs(std::move(es)) {}
      local_field_statement(const local_field_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = field_type[0].to_string();
        for (size_t i = 1; i < field_type.size(); ++i)
          result += "." + field_type[i].to_string();
        result += " " + names[0].to_string();
        if (xprs[0])
          result += " = " + xprs[0]->to_string();
        for (size_t i = 1; i < names.size(); ++i)
        {
          result += ", " + names[i].to_string();
          if (xprs[i])
            result += " = " + xprs[i]->to_string();
        }
        return result + ";";
      }

    protected:
      const std::vector<id_token> field_type;
      const std::vector<id_token> names;
      const std::vector<expression_ptr> xprs;
    };

    class assignment_statement final : public statement
    {
    public:
      assignment_statement(std::vector<id_token> is, const id_token &i, expression_ptr e) : ids(std::move(is)), id(i), xpr(std::move(e)) {}
      assignment_statement(const assignment_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        if (ids.empty())
          return id.to_string() + " = " + xpr->to_string() + ";";
        std::string result = ids[0].to_string();
        for (size_t i = 1; i < ids.size(); ++i)
          result += "." + ids[i].to_string();
        result += "." + id.to_string();
        result += " = " + xpr->to_string() + ";";
        return result;
      }

    protected:
      const std::vector<id_token> ids;
      const id_token id;
      expression_ptr xpr;
    };

    class expression_statement final : public statement
    {
    public:
      expression_statement(expression_ptr e) : xpr(std::move(e)) {}
      expression_statement(const expression_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override { return xpr->to_string() + ";"; }

    protected:
      expression_ptr xpr;
    };

    class disjunction_statement final : public statement
    {
    public:
      disjunction_statement(std::vector<std::vector<statement_ptr>> conjs, std::vector<expression_ptr> conj_costs) : conjunctions(std::move(conjs)), conjunction_costs(std::move(conj_costs)) {}
      disjunction_statement(const disjunction_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = "{\n";
        for (size_t j = 0; j < conjunctions[0].size(); ++j)
          result += "  " + conjunctions[0][j]->to_string() + "\n";
        result += "}";
        if (conjunction_costs[0])
          result += " [" + conjunction_costs[0]->to_string() + "]";
        for (size_t i = 1; i < conjunctions.size(); ++i)
        {
          result += " or {\n";
          for (size_t j = 0; j < conjunctions[i].size(); ++j)
            result += "  " + conjunctions[i][j]->to_string() + "\n";
          result += "}";
          if (conjunction_costs[i])
            result += " [" + conjunction_costs[i]->to_string() + "]";
        }
        return result;
      }

    protected:
      const std::vector<std::vector<statement_ptr>> conjunctions;
      const std::vector<expression_ptr> conjunction_costs;
    };

    class for_all_statement final : public statement
    {
    public:
      for_all_statement(std::vector<id_token> ft, const id_token &i, std::vector<statement_ptr> stmnts) : field_type(std::move(ft)), id(i), body(std::move(stmnts)) {}
      for_all_statement(const for_all_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = "for  (" + field_type[0].to_string();
        for (size_t i = 1; i < field_type.size(); ++i)
          result += "." + field_type[i].to_string();
        result += " " + id.to_string() + ") {\n";
        for (size_t i = 0; i < body.size(); ++i)
          result += "  " + body[i]->to_string() + "\n";
        return result + "}";
      }

    protected:
      const std::vector<id_token> field_type;
      const id_token id;
      const std::vector<statement_ptr> body;
    };

    class conjunction_statement final : public statement
    {
    public:
      conjunction_statement(std::vector<statement_ptr> stmnts) : body(std::move(stmnts)) {}
      conjunction_statement(const conjunction_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = "{\n";
        for (size_t i = 0; i < body.size(); ++i)
          result += "  " + body[i]->to_string() + "\n";
        return result + "}";
      }

    protected:
      const std::vector<statement_ptr> body;
    };

    class formula_statement final : public statement
    {
    public:
      formula_statement(const bool &isf, const id_token &fn, std::vector<id_token> scp, const id_token &pn, std::vector<id_token> assn_ns, std::vector<expression_ptr> assn_vs) : is_fact(isf), formula_name(fn), formula_scope(std::move(scp)), predicate_name(pn), assignment_names(std::move(assn_ns)), assignment_values(std::move(assn_vs)) {}
      formula_statement(const formula_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override
      {
        std::string result = (is_fact ? "fact" : "goal");
        result += " " + formula_name.to_string();
        if (!formula_scope.empty())
        {
          result += " = new " + formula_scope[0].to_string();
          for (size_t i = 1; i < formula_scope.size(); ++i)
            result += "." + formula_scope[i].to_string();
        }
        result += predicate_name.to_string() + "(";
        if (!assignment_names.empty())
        {
          result += assignment_names[0].to_string() + " = " + assignment_values[0]->to_string();
          for (size_t i = 1; i < assignment_names.size(); ++i)
            result += ", " + assignment_names[i].to_string() + " = " + assignment_values[i]->to_string();
        }
        return result + ");";
      }

    protected:
      const bool is_fact;
      const id_token formula_name;
      const std::vector<id_token> formula_scope;
      const id_token predicate_name;
      const std::vector<id_token> assignment_names;
      const std::vector<expression_ptr> assignment_values;
    };

    class return_statement final : public statement
    {
    public:
      return_statement(expression_ptr e) : xpr(std::move(e)) {}
      return_statement(const return_statement &orig) = delete;

      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const override;

      std::string to_string() const override { return "return " + xpr->to_string() + ";"; }

    protected:
      expression_ptr xpr;
    };

    class type_declaration
    {
    public:
      type_declaration() = default;
      type_declaration(const type_declaration &orig) = delete;
      virtual ~type_declaration() = default;

      virtual RIDDLE_EXPORT void declare(scope &) const = 0;
      virtual RIDDLE_EXPORT void refine(scope &) const = 0;
      virtual RIDDLE_EXPORT void refine_predicates(scope &) const = 0;

      virtual std::string to_string() const = 0;
    };
    using type_declaration_ptr = utils::u_ptr<const type_declaration>;

    class method_declaration final
    {
    public:
      method_declaration(std::vector<id_token> rt, const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<statement_ptr> stmnts) : return_type(std::move(rt)), name(n), parameters(std::move(pars)), body(std::move(stmnts)) {}
      method_declaration(const method_declaration &orig) = delete;

      RIDDLE_EXPORT void refine(scope &scp) const;

      std::string to_string() const
      {
        std::string result = return_type[0].to_string();
        for (size_t i = 1; i < return_type.size(); ++i)
          result += "." + return_type[i].to_string();
        result += ' ' + name.to_string() + '(';
        if (!parameters.empty())
        {
          result += parameters[0].first[0].to_string();
          for (size_t i = 1; i < parameters[0].first.size(); ++i)
            result += "." + parameters[0].first[i].to_string();
          result += ' ' + parameters[0].second.to_string();
          for (size_t i = 1; i < parameters.size(); ++i)
          {
            result += ", " + parameters[i].first[0].to_string();
            for (size_t j = 1; j < parameters[i].first.size(); ++j)
              result += "." + parameters[i].first[j].to_string();
            result += ' ' + parameters[i].second.to_string();
          }
        }
        result += ") {\n";
        for (size_t i = 0; i < body.size(); ++i)
          result += "  " + body[i]->to_string() + "\n";
        return result + "}";
      }

    protected:
      const std::vector<id_token> return_type;
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<statement_ptr> body;
    };
    using method_declaration_ptr = utils::u_ptr<const method_declaration>;

    class predicate_declaration final
    {
    public:
      predicate_declaration(const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<std::vector<id_token>> pl, std::vector<statement_ptr> stmnts) : name(n), parameters(std::move(pars)), predicate_list(std::move(pl)), body(std::move(stmnts)) {}
      predicate_declaration(const predicate_declaration &orig) = delete;

      RIDDLE_EXPORT void declare(scope &scp) const;
      RIDDLE_EXPORT void refine(scope &scp) const;

      std::string to_string() const
      {
        std::string result = "predicate " + name.to_string() + '(';
        if (!parameters.empty())
        {
          result += parameters[0].first[0].to_string();
          for (size_t i = 1; i < parameters[0].first.size(); ++i)
            result += "." + parameters[0].first[i].to_string();
          result += ' ' + parameters[0].second.to_string();
          for (size_t i = 1; i < parameters.size(); ++i)
          {
            result += ", " + parameters[i].first[0].to_string();
            for (size_t j = 1; j < parameters[i].first.size(); ++j)
              result += "." + parameters[i].first[j].to_string();
            result += ' ' + parameters[i].second.to_string();
          }
        }
        result += ")";
        if (!predicate_list.empty())
        {
          result += " : ";
          result += predicate_list[0][0].to_string();
          for (size_t i = 1; i < predicate_list[0].size(); ++i)
            result += ", " + predicate_list[0][i].to_string();
        }
        result += " {\n";
        for (size_t i = 0; i < body.size(); ++i)
          result += "  " + body[i]->to_string() + "\n";
        return result + "}";
      }

    protected:
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<std::vector<id_token>> predicate_list;
      const std::vector<statement_ptr> body;
    };
    using predicate_declaration_ptr = utils::u_ptr<const predicate_declaration>;

    class typedef_declaration final : public type_declaration
    {
    public:
      typedef_declaration(const id_token &n, const id_token &pt, expression_ptr e) : name(n), primitive_type(pt), xpr(std::move(e)) {}
      typedef_declaration(const typedef_declaration &orig) = delete;

      RIDDLE_EXPORT void declare(scope &scp) const override;
      RIDDLE_EXPORT void refine(scope &) const override {}
      RIDDLE_EXPORT void refine_predicates(scope &) const override {}

      std::string to_string() const override { return "typedef " + primitive_type.to_string() + " " + name.to_string() + " = " + xpr->to_string() + ";"; }

    protected:
      const id_token name;
      const id_token primitive_type;
      expression_ptr xpr;
    };

    class enum_declaration final : public type_declaration
    {
    public:
      enum_declaration(const id_token &n, std::vector<string_token> es, std::vector<std::vector<id_token>> trs) : name(n), enums(std::move(es)), type_refs(std::move(trs)) {}
      enum_declaration(const enum_declaration &orig) = delete;

      RIDDLE_EXPORT void declare(scope &scp) const override;
      RIDDLE_EXPORT void refine(scope &scp) const override;
      RIDDLE_EXPORT void refine_predicates(scope &) const override {}

      std::string to_string() const override
      {
        std::string result = "enum " + name.to_string() + " {";
        if (!enums.empty())
        {
          result += '\"' + enums[0].to_string() + '\"';
          for (size_t i = 1; i < enums.size(); ++i)
            result += ", \"" + enums[i].to_string() + '\"';
          if (!type_refs.empty())
            result += ", ";
        }
        if (!type_refs.empty())
        {
          result += type_refs[0][0].to_string();
          for (size_t i = 1; i < type_refs[0].size(); ++i)
            result += "." + type_refs[0][i].to_string();
          for (size_t i = 1; i < type_refs.size(); ++i)
          {
            result += ", " + type_refs[i][0].to_string();
            for (size_t j = 1; j < type_refs[i].size(); ++j)
              result += "." + type_refs[i][j].to_string();
          }
        }
        return result + "}";
      }

    protected:
      const id_token name;
      const std::vector<string_token> enums;
      const std::vector<std::vector<id_token>> type_refs;
    };

    class variable_declaration final
    {
      friend class field_declaration;

    public:
      variable_declaration(const id_token &n, expression_ptr e = nullptr) : name(n), xpr(std::move(e)) {}
      variable_declaration(const variable_declaration &orig) = delete;

      std::string to_string() const { return name.to_string() + (xpr ? " = " + xpr->to_string() : ""); }

    protected:
      const id_token name;
      expression_ptr xpr;
    };
    using variable_declaration_ptr = utils::u_ptr<const variable_declaration>;

    class field_declaration final
    {
    public:
      field_declaration(std::vector<id_token> tp, std::vector<variable_declaration_ptr> ds) : field_type(std::move(tp)), declarations(std::move(ds)) {}
      field_declaration(const field_declaration &orig) = delete;

      RIDDLE_EXPORT void refine(scope &scp) const;

      std::string to_string() const
      {
        std::string result = field_type[0].to_string();
        for (size_t i = 1; i < field_type.size(); ++i)
          result += "." + field_type[i].to_string();
        result += ' ' + declarations[0]->to_string();
        for (size_t i = 1; i < declarations.size(); ++i)
          result += ", " + declarations[i]->to_string();
        return result + ";";
      }

    protected:
      const std::vector<id_token> field_type;
      const std::vector<variable_declaration_ptr> declarations;
    };
    using field_declaration_ptr = utils::u_ptr<const field_declaration>;

    class constructor_declaration final
    {
    public:
      constructor_declaration(std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<id_token> ins, std::vector<std::vector<expression_ptr>> ivs, std::vector<statement_ptr> stmnts) : parameters(std::move(pars)), init_names(std::move(ins)), init_vals(std::move(ivs)), body(std::move(stmnts)) {}
      constructor_declaration(const constructor_declaration &orig) = delete;

      RIDDLE_EXPORT void refine(scope &scp) const;

      std::string to_string() const
      {
        std::string result = "(";
        if (!parameters.empty())
        {
          result += parameters[0].first[0].to_string() + " " + parameters[0].second.to_string();
          for (size_t i = 1; i < parameters[0].first.size(); ++i)
            result += "." + parameters[0].first[i].to_string();
          for (size_t i = 1; i < parameters.size(); ++i)
          {
            result += ", " + parameters[i].first[0].to_string() + " " + parameters[i].second.to_string();
            for (size_t j = 1; j < parameters[i].first.size(); ++j)
              result += "." + parameters[i].first[j].to_string();
          }
        }
        result += ")";
        if (!init_names.empty())
        {
          result += " : " + init_names[0].to_string() + '(';
          if (!init_vals[0].empty())
          {
            result += init_vals[0][0]->to_string();
            for (size_t i = 1; i < init_vals[0].size(); ++i)
              result += ", " + init_vals[0][i]->to_string();
          }
          result += ')';
          for (size_t i = 1; i < init_names.size(); ++i)
          {
            result += ", " + init_names[i].to_string() + '(';
            if (!init_vals[i].empty())
            {
              result += init_vals[i][0]->to_string();
              for (size_t j = 1; j < init_vals[i].size(); ++j)
                result += ", " + init_vals[i][j]->to_string();
            }
            result += ')';
          }
        }
        result += " {";
        if (!body.empty())
        {
          result += body[0]->to_string();
          for (size_t i = 1; i < body.size(); ++i)
            result += body[i]->to_string();
        }
        return result + "}";
      }

    protected:
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<id_token> init_names;
      const std::vector<std::vector<expression_ptr>> init_vals;
      const std::vector<statement_ptr> body;
    };
    using constructor_declaration_ptr = utils::u_ptr<const constructor_declaration>;

    class class_declaration final : public type_declaration
    {
    public:
      class_declaration(const id_token &n, std::vector<std::vector<id_token>> bcs, std::vector<field_declaration_ptr> fs, std::vector<constructor_declaration_ptr> cs, std::vector<method_declaration_ptr> ms, std::vector<predicate_declaration_ptr> ps, std::vector<type_declaration_ptr> ts) : name(n), base_classes(std::move(bcs)), fields(std::move(fs)), constructors(std::move(cs)), methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)) {}
      class_declaration(const class_declaration &orig) = delete;

      RIDDLE_EXPORT void declare(scope &scp) const override;
      RIDDLE_EXPORT void refine(scope &scp) const override;
      RIDDLE_EXPORT void refine_predicates(scope &scp) const override;

      std::string to_string() const override
      {
        std::string result = "class " + name.to_string();
        if (!base_classes.empty())
        {
          result += " : " + base_classes[0][0].to_string();
          for (size_t i = 1; i < base_classes[0].size(); ++i)
            result += "." + base_classes[0][i].to_string();
          for (size_t i = 1; i < base_classes.size(); ++i)
          {
            result += ", " + base_classes[i][0].to_string();
            for (size_t j = 1; j < base_classes[i].size(); ++j)
              result += "." + base_classes[i][j].to_string();
          }
        }
        result += " {";
        if (!fields.empty())
        {
          result += fields[0]->to_string();
          for (size_t i = 1; i < fields.size(); ++i)
            result += fields[i]->to_string();
        }
        if (!constructors.empty())
        {
          result += constructors[0]->to_string();
          for (size_t i = 1; i < constructors.size(); ++i)
            result += constructors[i]->to_string();
        }
        if (!methods.empty())
        {
          result += methods[0]->to_string();
          for (size_t i = 1; i < methods.size(); ++i)
            result += methods[i]->to_string();
        }
        if (!predicates.empty())
        {
          result += predicates[0]->to_string();
          for (size_t i = 1; i < predicates.size(); ++i)
            result += predicates[i]->to_string();
        }
        if (!types.empty())
        {
          result += types[0]->to_string();
          for (size_t i = 1; i < types.size(); ++i)
            result += types[i]->to_string();
        }
        return result + "}";
      }

    protected:
      const id_token name;
      const std::vector<std::vector<id_token>> base_classes;
      const std::vector<field_declaration_ptr> fields;
      const std::vector<constructor_declaration_ptr> constructors;
      const std::vector<method_declaration_ptr> methods;
      const std::vector<predicate_declaration_ptr> predicates;
      const std::vector<type_declaration_ptr> types;
    };

    class compilation_unit final
    {
    public:
      compilation_unit(std::vector<method_declaration_ptr> ms, std::vector<predicate_declaration_ptr> ps, std::vector<type_declaration_ptr> ts, std::vector<statement_ptr> stmnts) : methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)), body(std::move(stmnts)) {}
      compilation_unit(const compilation_unit &orig) = delete;

      RIDDLE_EXPORT void declare(scope &scp) const;
      RIDDLE_EXPORT void refine(scope &scp) const;
      RIDDLE_EXPORT void refine_predicates(scope &scp) const;
      RIDDLE_EXPORT void execute(scope &scp, env &ctx) const;

      std::string to_string() const
      {
        std::string result = "";
        if (!methods.empty())
        {
          result += methods[0]->to_string();
          for (size_t i = 1; i < methods.size(); ++i)
            result += methods[i]->to_string();
        }
        if (!predicates.empty())
        {
          result += predicates[0]->to_string();
          for (size_t i = 1; i < predicates.size(); ++i)
            result += predicates[i]->to_string();
        }
        if (!types.empty())
        {
          result += types[0]->to_string();
          for (size_t i = 1; i < types.size(); ++i)
            result += types[i]->to_string();
        }
        if (!body.empty())
        {
          result += body[0]->to_string();
          for (size_t i = 1; i < body.size(); ++i)
            result += body[i]->to_string();
        }
        return result;
      }

    protected:
      const std::vector<method_declaration_ptr> methods;
      const std::vector<predicate_declaration_ptr> predicates;
      const std::vector<type_declaration_ptr> types;
      const std::vector<statement_ptr> body;
    };
    using compilation_unit_ptr = utils::u_ptr<const compilation_unit>;
  } // namespace ast

  class parser
  {
  public:
    RIDDLE_EXPORT parser(const std::string &str);
    RIDDLE_EXPORT parser(std::istream &is);
    parser(const parser &orig) = delete;
    RIDDLE_EXPORT virtual ~parser();

    RIDDLE_EXPORT ast::compilation_unit_ptr parse();

  private:
    const token *next();
    bool match(const symbol &sym);
    void backtrack(const size_t &p) noexcept;

    ast::compilation_unit_ptr _compilation_unit();
    const ast::typedef_declaration *_typedef_declaration();
    const ast::enum_declaration *_enum_declaration();
    const ast::class_declaration *_class_declaration();
    ast::field_declaration_ptr _field_declaration();
    ast::method_declaration_ptr _method_declaration();
    ast::constructor_declaration_ptr _constructor_declaration();
    ast::predicate_declaration_ptr _predicate_declaration();
    ast::statement_ptr _statement();
    ast::expression_ptr _expression(const size_t &pr = 0);

    void error(const std::string &err);

  private:
    lexer lex;                                  // the current lexer..
    const token *tk = nullptr;                  // the current lookahead token..
    std::vector<utils::u_ptr<const token>> tks; // all the tokens parsed so far..
    size_t pos = 0;                             // the current position within tks'..
  };
} // namespace riddle