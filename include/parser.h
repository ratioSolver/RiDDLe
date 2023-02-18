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
    };
    using expression_ptr = utils::u_ptr<const expression>;

    class bool_literal_expression final : public expression
    {
    public:
      bool_literal_expression(const bool_token &l) : literal(l) {}
      bool_literal_expression(const bool_literal_expression &orig) = delete;
      virtual ~bool_literal_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const bool_token literal;
    };

    class int_literal_expression final : public expression
    {
    public:
      int_literal_expression(const int_token &l) : literal(l) {}
      int_literal_expression(const int_literal_expression &orig) = delete;
      virtual ~int_literal_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const int_token literal;
    };

    class real_literal_expression final : public expression
    {
    public:
      real_literal_expression(const real_token &l) : literal(l) {}
      real_literal_expression(const real_literal_expression &orig) = delete;
      virtual ~real_literal_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const real_token literal;
    };

    class string_literal_expression final : public expression
    {
    public:
      string_literal_expression(const string_token &l) : literal(l) {}
      string_literal_expression(const string_literal_expression &orig) = delete;
      virtual ~string_literal_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const string_token literal;
    };

    class cast_expression final : public expression
    {
    public:
      cast_expression(std::vector<id_token> tp, expression_ptr e) : cast_to_type(std::move(tp)), xpr(std::move(e)) {}
      cast_expression(const cast_expression &orig) = delete;
      virtual ~cast_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<id_token> cast_to_type;
      const expression_ptr xpr;
    };

    class plus_expression final : public expression
    {
    public:
      plus_expression(expression_ptr e) : xpr(std::move(e)) {}
      plus_expression(const plus_expression &orig) = delete;
      virtual ~plus_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr xpr;
    };

    class minus_expression final : public expression
    {
    public:
      minus_expression(expression_ptr e) : xpr(std::move(e)) {}
      minus_expression(const minus_expression &orig) = delete;
      virtual ~minus_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr xpr;
    };

    class not_expression final : public expression
    {
    public:
      not_expression(expression_ptr e) : xpr(std::move(e)) {}
      not_expression(const not_expression &orig) = delete;
      virtual ~not_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr xpr;
    };

    class constructor_expression final : public expression
    {
    public:
      constructor_expression(std::vector<id_token> it, std::vector<expression_ptr> es) : instance_type(std::move(it)), expressions(std::move(es)) {}
      constructor_expression(const constructor_expression &orig) = delete;
      virtual ~constructor_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<id_token> instance_type;
      const std::vector<expression_ptr> expressions;
    };

    class eq_expression final : public expression
    {
    public:
      eq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      eq_expression(const eq_expression &orig) = delete;
      virtual ~eq_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class neq_expression final : public expression
    {
    public:
      neq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      neq_expression(const neq_expression &orig) = delete;
      virtual ~neq_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class lt_expression final : public expression
    {
    public:
      lt_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      lt_expression(const lt_expression &orig) = delete;
      virtual ~lt_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class leq_expression final : public expression
    {
    public:
      leq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      leq_expression(const leq_expression &orig) = delete;
      virtual ~leq_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class geq_expression final : public expression
    {
    public:
      geq_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      geq_expression(const geq_expression &orig) = delete;
      virtual ~geq_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class gt_expression final : public expression
    {
    public:
      gt_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      gt_expression(const gt_expression &orig) = delete;
      virtual ~gt_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class function_expression final : public expression
    {
    public:
      function_expression(std::vector<id_token> is, const id_token &fn, std::vector<expression_ptr> es) : ids(std::move(is)), function_name(std::move(fn)), expressions(std::move(es)) {}
      function_expression(const function_expression &orig) = delete;
      virtual ~function_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

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
      virtual ~id_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<id_token> ids;
    };

    class implication_expression final : public expression
    {
    public:
      implication_expression(expression_ptr l, expression_ptr r) : left(std::move(l)), right(std::move(r)) {}
      implication_expression(const implication_expression &orig) = delete;
      virtual ~implication_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const expression_ptr left;
      const expression_ptr right;
    };

    class disjunction_expression final : public expression
    {
    public:
      disjunction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      disjunction_expression(const disjunction_expression &orig) = delete;
      virtual ~disjunction_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class conjunction_expression final : public expression
    {
    public:
      conjunction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      conjunction_expression(const conjunction_expression &orig) = delete;
      virtual ~conjunction_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class exct_one_expression final : public expression
    {
    public:
      exct_one_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      exct_one_expression(const exct_one_expression &orig) = delete;
      virtual ~exct_one_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class addition_expression final : public expression
    {
    public:
      addition_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      addition_expression(const addition_expression &orig) = delete;
      virtual ~addition_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class subtraction_expression final : public expression
    {
    public:
      subtraction_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      subtraction_expression(const subtraction_expression &orig) = delete;
      virtual ~subtraction_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class multiplication_expression final : public expression
    {
    public:
      multiplication_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      multiplication_expression(const multiplication_expression &orig) = delete;
      virtual ~multiplication_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

    protected:
      const std::vector<expression_ptr> expressions;
    };

    class division_expression final : public expression
    {
    public:
      division_expression(std::vector<expression_ptr> es) : expressions(std::move(es)) {}
      division_expression(const division_expression &orig) = delete;
      virtual ~division_expression() = default;

      expr evaluate(scope &scp, env &ctx) const override;

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
    };
    using statement_ptr = utils::u_ptr<const statement>;

    class local_field_statement final : public statement
    {
    public:
      local_field_statement(std::vector<id_token> ft, std::vector<id_token> ns, std::vector<expression_ptr> es) : field_type(std::move(ft)), names(std::move(ns)), xprs(std::move(es)) {}
      local_field_statement(const local_field_statement &orig) = delete;
      virtual ~local_field_statement() = default;

      void execute(scope &scp, env &ctx) const override;

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
      virtual ~assignment_statement() = default;

      void execute(scope &scp, env &ctx) const override;

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
      virtual ~expression_statement() = default;

      void execute(scope &scp, env &ctx) const override;

    protected:
      expression_ptr xpr;
    };

    class disjunction_statement final : public statement
    {
    public:
      disjunction_statement(std::vector<std::vector<statement_ptr>> conjs, std::vector<expression_ptr> conj_costs) : conjunctions(std::move(conjs)), conjunction_costs(std::move(conj_costs)) {}
      disjunction_statement(const disjunction_statement &orig) = delete;
      virtual ~disjunction_statement() = default;

      void execute(scope &scp, env &ctx) const override;

    protected:
      const std::vector<std::vector<statement_ptr>> conjunctions;
      const std::vector<expression_ptr> conjunction_costs;
    };

    class conjunction_statement final : public statement
    {
    public:
      conjunction_statement(std::vector<statement_ptr> stmnts) : body(std::move(stmnts)) {}
      conjunction_statement(const conjunction_statement &orig) = delete;
      virtual ~conjunction_statement() = default;

      void execute(scope &scp, env &ctx) const override;

    protected:
      const std::vector<statement_ptr> body;
    };

    class formula_statement final : public statement
    {
    public:
      formula_statement(const bool &isf, const id_token &fn, std::vector<id_token> scp, const id_token &pn, std::vector<id_token> assn_ns, std::vector<expression_ptr> assn_vs) : is_fact(isf), formula_name(fn), formula_scope(std::move(scp)), predicate_name(pn), assignment_names(std::move(assn_ns)), assignment_values(std::move(assn_vs)) {}
      formula_statement(const formula_statement &orig) = delete;
      virtual ~formula_statement() = default;

      void execute(scope &scp, env &ctx) const override;

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
      virtual ~return_statement() = default;

      void execute(scope &scp, env &ctx) const override;

    protected:
      expression_ptr xpr;
    };

    class type_declaration
    {
    public:
      type_declaration() {}
      type_declaration(const type_declaration &orig) = delete;
      virtual ~type_declaration() {}
    };
    using type_declaration_ptr = utils::u_ptr<const type_declaration>;

    class method_declaration
    {
    public:
      method_declaration(std::vector<id_token> rt, const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<statement_ptr> stmnts) : return_type(std::move(rt)), name(n), parameters(std::move(pars)), body(std::move(stmnts)) {}
      method_declaration(const method_declaration &orig) = delete;
      virtual ~method_declaration() = default;

    protected:
      const std::vector<id_token> return_type;
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<statement_ptr> body;
    };
    using method_declaration_ptr = utils::u_ptr<const method_declaration>;

    class predicate_declaration
    {
    public:
      predicate_declaration(const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<std::vector<id_token>> pl, std::vector<statement_ptr> stmnts) : name(n), parameters(std::move(pars)), predicate_list(std::move(pl)), body(std::move(stmnts)) {}
      predicate_declaration(const predicate_declaration &orig) = delete;
      virtual ~predicate_declaration() = default;

    protected:
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<std::vector<id_token>> predicate_list;
      const std::vector<statement_ptr> body;
    };
    using predicate_declaration_ptr = utils::u_ptr<const predicate_declaration>;

    class typedef_declaration : public type_declaration
    {
    public:
      typedef_declaration(const id_token &n, const id_token &pt, expression_ptr e) : name(n), primitive_type(pt), xpr(std::move(e)) {}
      typedef_declaration(const typedef_declaration &orig) = delete;
      virtual ~typedef_declaration() = default;

    protected:
      const id_token name;
      const id_token primitive_type;
      expression_ptr xpr;
    };

    class enum_declaration : public type_declaration
    {
    public:
      enum_declaration(const id_token &n, std::vector<string_token> es, std::vector<std::vector<id_token>> trs) : name(n), enums(std::move(es)), type_refs(std::move(trs)) {}
      enum_declaration(const enum_declaration &orig) = delete;
      virtual ~enum_declaration() = default;

    protected:
      const id_token name;
      const std::vector<string_token> enums;
      const std::vector<std::vector<id_token>> type_refs;
    };

    class variable_declaration
    {
      friend class field_declaration;

    public:
      variable_declaration(const id_token &n, expression_ptr e = nullptr) : name(n), xpr(std::move(e)) {}
      variable_declaration(const variable_declaration &orig) = delete;
      virtual ~variable_declaration() = default;

    protected:
      const id_token name;
      expression_ptr xpr;
    };
    using variable_declaration_ptr = utils::u_ptr<const variable_declaration>;

    class field_declaration
    {
    public:
      field_declaration(std::vector<id_token> tp, std::vector<variable_declaration_ptr> ds) : field_type(std::move(tp)), declarations(std::move(ds)) {}
      field_declaration(const field_declaration &orig) = delete;
      virtual ~field_declaration() = default;

    protected:
      const std::vector<id_token> field_type;
      const std::vector<variable_declaration_ptr> declarations;
    };
    using field_declaration_ptr = utils::u_ptr<const field_declaration>;

    class constructor_declaration
    {
    public:
      constructor_declaration(std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<id_token> ins, std::vector<std::vector<expression_ptr>> ivs, std::vector<statement_ptr> stmnts) : parameters(std::move(pars)), init_names(std::move(ins)), init_vals(std::move(ivs)), body(std::move(stmnts)) {}
      constructor_declaration(const constructor_declaration &orig) = delete;
      virtual ~constructor_declaration() = default;

    protected:
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<id_token> init_names;
      const std::vector<std::vector<expression_ptr>> init_vals;
      const std::vector<statement_ptr> body;
    };
    using constructor_declaration_ptr = utils::u_ptr<const constructor_declaration>;

    class class_declaration : public type_declaration
    {
    public:
      class_declaration(const id_token &n, std::vector<std::vector<id_token>> bcs, std::vector<field_declaration_ptr> fs, std::vector<constructor_declaration_ptr> cs, std::vector<method_declaration_ptr> ms, std::vector<predicate_declaration_ptr> ps, std::vector<type_declaration_ptr> ts) : name(n), base_classes(std::move(bcs)), fields(std::move(fs)), constructors(std::move(cs)), methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)) {}
      class_declaration(const class_declaration &orig) = delete;
      virtual ~class_declaration() = default;

    protected:
      const id_token name;
      const std::vector<std::vector<id_token>> base_classes;
      const std::vector<field_declaration_ptr> fields;
      const std::vector<constructor_declaration_ptr> constructors;
      const std::vector<method_declaration_ptr> methods;
      const std::vector<predicate_declaration_ptr> predicates;
      const std::vector<type_declaration_ptr> types;
    };

    class compilation_unit
    {
    public:
      compilation_unit(std::vector<method_declaration_ptr> ms, std::vector<predicate_declaration_ptr> ps, std::vector<type_declaration_ptr> ts, std::vector<statement_ptr> stmnts) : methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)), body(std::move(stmnts)) {}
      compilation_unit(const compilation_unit &orig) = delete;
      virtual ~compilation_unit() = default;

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

    const ast::typedef_declaration *_typedef_declaration();
    const ast::enum_declaration *_enum_declaration();
    const ast::class_declaration *_class_declaration();
    const ast::field_declaration *_field_declaration();
    const ast::method_declaration *_method_declaration();
    const ast::constructor_declaration *_constructor_declaration();
    const ast::predicate_declaration *_predicate_declaration();
    const ast::statement *_statement();
    ast::expression_ptr _expression(const size_t &pr = 0);

    void error(const std::string &err);

    /**
     * The declarations.
     */
    const ast::method_declaration *new_method_declaration(std::vector<id_token> rt, const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<ast::statement_ptr> stmnts) const noexcept { return new const ast::method_declaration(std::move(rt), n, std::move(pars), std::move(stmnts)); }
    const ast::predicate_declaration *new_predicate_declaration(const id_token &n, std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<std::vector<id_token>> pl, std::vector<ast::statement_ptr> stmnts) const noexcept { return new const ast::predicate_declaration(n, std::move(pars), std::move(pl), std::move(stmnts)); }
    const ast::typedef_declaration *new_typedef_declaration(const id_token &n, const id_token &pt, ast::expression_ptr e) const noexcept { return new const ast::typedef_declaration(n, pt, std::move(e)); }
    const ast::enum_declaration *new_enum_declaration(const id_token &n, std::vector<string_token> es, std::vector<std::vector<id_token>> trs) const noexcept { return new const ast::enum_declaration(n, std::move(es), std::move(trs)); }
    const ast::class_declaration *new_class_declaration(const id_token &n, std::vector<std::vector<id_token>> bcs, std::vector<ast::field_declaration_ptr> fs, std::vector<ast::constructor_declaration_ptr> cs, std::vector<ast::method_declaration_ptr> ms, std::vector<ast::predicate_declaration_ptr> ps, std::vector<ast::type_declaration_ptr> ts) const noexcept { return new const ast::class_declaration(n, std::move(bcs), std::move(fs), std::move(cs), std::move(ms), std::move(ps), std::move(ts)); }
    const ast::variable_declaration *new_variable_declaration(const id_token &n, ast::expression_ptr e = nullptr) const noexcept { return new const ast::variable_declaration(n, std::move(e)); }
    const ast::field_declaration *new_field_declaration(std::vector<id_token> tp, std::vector<ast::variable_declaration_ptr> ds) const noexcept { return new const ast::field_declaration(std::move(tp), std::move(ds)); }
    const ast::constructor_declaration *new_constructor_declaration(std::vector<std::pair<const std::vector<id_token>, const id_token>> pars, std::vector<id_token> ins, std::vector<std::vector<ast::expression_ptr>> ivs, std::vector<ast::statement_ptr> stmnts) const noexcept { return new const ast::constructor_declaration(std::move(pars), std::move(ins), std::move(ivs), std::move(stmnts)); }
    const ast::compilation_unit *new_compilation_unit(std::vector<ast::method_declaration_ptr> ms, std::vector<ast::predicate_declaration_ptr> ps, std::vector<ast::type_declaration_ptr> ts, std::vector<ast::statement_ptr> stmnts) const noexcept { return new const ast::compilation_unit(std::move(ms), std::move(ps), std::move(ts), std::move(stmnts)); }

    /**
     * The statements.
     */
    const ast::local_field_statement *new_local_field_statement(std::vector<id_token> ft, std::vector<id_token> ns, std::vector<ast::expression_ptr> es) const noexcept { return new const ast::local_field_statement(std::move(ft), std::move(ns), std::move(es)); }
    const ast::assignment_statement *new_assignment_statement(std::vector<id_token> is, const id_token &i, ast::expression_ptr e) const noexcept { return new const ast::assignment_statement(std::move(is), i, std::move(e)); }
    const ast::expression_statement *new_expression_statement(ast::expression_ptr e) const noexcept { return new const ast::expression_statement(std::move(e)); }
    const ast::disjunction_statement *new_disjunction_statement(std::vector<std::vector<ast::statement_ptr>> conjs, std::vector<ast::expression_ptr> conj_costs) const noexcept { return new const ast::disjunction_statement(std::move(conjs), std::move(conj_costs)); }
    const ast::conjunction_statement *new_conjunction_statement(std::vector<ast::statement_ptr> stmnts) const noexcept { return new const ast::conjunction_statement(std::move(stmnts)); }
    const ast::formula_statement *new_formula_statement(const bool &isf, const id_token &fn, std::vector<id_token> scp, const id_token &pn, std::vector<id_token> assn_ns, std::vector<ast::expression_ptr> assn_vs) const noexcept { return new const ast::formula_statement(isf, fn, std::move(scp), pn, std::move(assn_ns), std::move(assn_vs)); }
    const ast::return_statement *new_return_statement(ast::expression_ptr e) const noexcept { return new const ast::return_statement(std::move(e)); }

    /**
     * The expressions.
     */
    const ast::bool_literal_expression *new_bool_literal_expression(const bool_token &l) const noexcept { return new const ast::bool_literal_expression(l); }
    const ast::int_literal_expression *new_int_literal_expression(const int_token &l) const noexcept { return new const ast::int_literal_expression(l); }
    const ast::real_literal_expression *new_real_literal_expression(const real_token &l) const noexcept { return new const ast::real_literal_expression(l); }
    const ast::string_literal_expression *new_string_literal_expression(const string_token &l) const noexcept { return new const ast::string_literal_expression(l); }
    const ast::cast_expression *new_cast_expression(std::vector<id_token> tp, ast::expression_ptr e) const noexcept { return new const ast::cast_expression(std::move(tp), std::move(e)); }
    const ast::plus_expression *new_plus_expression(ast::expression_ptr e) const noexcept { return new const ast::plus_expression(std::move(e)); }
    const ast::minus_expression *new_minus_expression(ast::expression_ptr e) const noexcept { return new const ast::minus_expression(std::move(e)); }
    const ast::not_expression *new_not_expression(ast::expression_ptr e) const noexcept { return new const ast::not_expression(std::move(e)); }
    const ast::constructor_expression *new_constructor_expression(std::vector<id_token> it, std::vector<ast::expression_ptr> es) const noexcept { return new const ast::constructor_expression(std::move(it), std::move(es)); }
    const ast::eq_expression *new_eq_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::eq_expression(std::move(l), std::move(r)); }
    const ast::neq_expression *new_neq_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::neq_expression(std::move(l), std::move(r)); }
    const ast::lt_expression *new_lt_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::lt_expression(std::move(l), std::move(r)); }
    const ast::leq_expression *new_leq_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::leq_expression(std::move(l), std::move(r)); }
    const ast::geq_expression *new_geq_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::geq_expression(std::move(l), std::move(r)); }
    const ast::gt_expression *new_gt_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::gt_expression(std::move(l), std::move(r)); }
    const ast::function_expression *new_function_expression(std::vector<id_token> is, const id_token &fn, std::vector<ast::expression_ptr> es) const noexcept { return new const ast::function_expression(std::move(is), fn, std::move(es)); }
    const ast::id_expression *new_id_expression(std::vector<id_token> is) const noexcept { return new const ast::id_expression(std::move(is)); }
    const ast::implication_expression *new_implication_expression(ast::expression_ptr l, ast::expression_ptr r) const noexcept { return new const ast::implication_expression(std::move(l), std::move(r)); }
    const ast::disjunction_expression *new_disjunction_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::disjunction_expression(std::move(es)); }
    const ast::conjunction_expression *new_conjunction_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::conjunction_expression(std::move(es)); }
    const ast::exct_one_expression *new_exct_one_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::exct_one_expression(std::move(es)); }
    const ast::addition_expression *new_addition_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::addition_expression(std::move(es)); }
    const ast::subtraction_expression *new_subtraction_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::subtraction_expression(std::move(es)); }
    const ast::multiplication_expression *new_multiplication_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::multiplication_expression(std::move(es)); }
    const ast::division_expression *new_division_expression(std::vector<ast::expression_ptr> es) const noexcept { return new const ast::division_expression(std::move(es)); }

  private:
    lexer lex;                                  // the current lexer..
    const token *tk = nullptr;                  // the current lookahead token..
    std::vector<utils::u_ptr<const token>> tks; // all the tokens parsed so far..
    size_t pos = 0;                             // the current position within tks'..
  };
} // namespace riddle