#pragma once

#include "riddle_lexer.h"
#include <vector>
#include <memory>

namespace riddle
{
  namespace ast
  {
    class expression
    {
    public:
      expression() = default;
      expression(const expression &orig) = delete;
      virtual ~expression() = default;
    };

    class bool_literal_expression : public expression
    {
    public:
      RIDDLE_EXPORT bool_literal_expression(const bool_token &l) : literal(l) {}
      bool_literal_expression(const bool_literal_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~bool_literal_expression() = default;

    protected:
      const bool_token literal;
    };

    class int_literal_expression : public expression
    {
    public:
      RIDDLE_EXPORT int_literal_expression(const int_token &l) : literal(l) {}
      int_literal_expression(const int_literal_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~int_literal_expression() = default;

    protected:
      const int_token literal;
    };

    class real_literal_expression : public expression
    {
    public:
      RIDDLE_EXPORT real_literal_expression(const real_token &l) : literal(l) {}
      real_literal_expression(const real_literal_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~real_literal_expression() = default;

    protected:
      const real_token literal;
    };

    class string_literal_expression : public expression
    {
    public:
      RIDDLE_EXPORT string_literal_expression(const string_token &l) : literal(l) {}
      string_literal_expression(const string_literal_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~string_literal_expression() = default;

    protected:
      const string_token literal;
    };

    class cast_expression : public expression
    {
    public:
      cast_expression(const std::vector<id_token> &tp, std::unique_ptr<const expression> e) : cast_to_type(tp), xpr(std::move(e)) {}
      cast_expression(const cast_expression &orig) = delete;
      virtual ~cast_expression() = default;

    protected:
      const std::vector<id_token> cast_to_type;
      std::unique_ptr<const expression> xpr;
    };

    class plus_expression : public expression
    {
    public:
      plus_expression(std::unique_ptr<const expression> e) : xpr(std::move(e)) {}
      plus_expression(const plus_expression &orig) = delete;
      virtual ~plus_expression() = default;

    protected:
      std::unique_ptr<const expression> xpr;
    };

    class minus_expression : public expression
    {
    public:
      minus_expression(std::unique_ptr<const expression> e) : xpr(std::move(e)) {}
      minus_expression(const minus_expression &orig) = delete;
      virtual ~minus_expression() = default;

    protected:
      std::unique_ptr<const expression> xpr;
    };

    class not_expression : public expression
    {
    public:
      not_expression(std::unique_ptr<const expression> e) : xpr(std::move(e)) {}
      not_expression(const not_expression &orig) = delete;
      virtual ~not_expression() = default;

    protected:
      std::unique_ptr<const expression> xpr;
    };

    class constructor_expression : public expression
    {
    public:
      constructor_expression(const std::vector<id_token> &it, std::vector<std::unique_ptr<const expression>> es) : instance_type(it), expressions(std::move(es)) {}
      constructor_expression(const constructor_expression &orig) = delete;
      virtual ~constructor_expression() = default;

    protected:
      const std::vector<id_token> instance_type;
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class eq_expression : public expression
    {
    public:
      eq_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      eq_expression(const eq_expression &orig) = delete;
      virtual ~eq_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class neq_expression : public expression
    {
    public:
      neq_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      neq_expression(const neq_expression &orig) = delete;
      virtual ~neq_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class lt_expression : public expression
    {
    public:
      RIDDLE_EXPORT lt_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      lt_expression(const lt_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~lt_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class leq_expression : public expression
    {
    public:
      RIDDLE_EXPORT leq_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      leq_expression(const leq_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~leq_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class geq_expression : public expression
    {
    public:
      RIDDLE_EXPORT geq_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      geq_expression(const geq_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~geq_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class gt_expression : public expression
    {
    public:
      RIDDLE_EXPORT gt_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      gt_expression(const gt_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~gt_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class function_expression : public expression
    {
    public:
      function_expression(const std::vector<id_token> &is, const id_token &fn, std::vector<std::unique_ptr<const expression>> es) : ids(is), function_name(fn), expressions(std::move(es)) {}
      function_expression(const function_expression &orig) = delete;
      virtual ~function_expression() = default;

    protected:
      const std::vector<id_token> ids;
      const id_token function_name;
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class id_expression : public expression
    {
    public:
      RIDDLE_EXPORT id_expression(const std::vector<id_token> &is) : ids(is) {}
      id_expression(const id_expression &orig) = delete;
      RIDDLE_EXPORT virtual ~id_expression() = default;

    protected:
      const std::vector<id_token> ids;
    };

    class implication_expression : public expression
    {
    public:
      implication_expression(std::unique_ptr<const expression> l, std::unique_ptr<const expression> r) : left(std::move(l)), right(std::move(r)) {}
      implication_expression(const implication_expression &orig) = delete;
      virtual ~implication_expression() = default;

    protected:
      std::unique_ptr<const expression> left;
      std::unique_ptr<const expression> right;
    };

    class disjunction_expression : public expression
    {
    public:
      disjunction_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      disjunction_expression(const disjunction_expression &orig) = delete;
      virtual ~disjunction_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class conjunction_expression : public expression
    {
    public:
      conjunction_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      conjunction_expression(const conjunction_expression &orig) = delete;
      virtual ~conjunction_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class exct_one_expression : public expression
    {
    public:
      exct_one_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      exct_one_expression(const exct_one_expression &orig) = delete;
      virtual ~exct_one_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class addition_expression : public expression
    {
    public:
      addition_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      addition_expression(const addition_expression &orig) = delete;
      virtual ~addition_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class subtraction_expression : public expression
    {
    public:
      subtraction_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      subtraction_expression(const subtraction_expression &orig) = delete;
      virtual ~subtraction_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class multiplication_expression : public expression
    {
    public:
      multiplication_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      multiplication_expression(const multiplication_expression &orig) = delete;
      virtual ~multiplication_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class division_expression : public expression
    {
    public:
      division_expression(std::vector<std::unique_ptr<const expression>> es) : expressions(std::move(es)) {}
      division_expression(const division_expression &orig) = delete;
      virtual ~division_expression() = default;

    protected:
      const std::vector<std::unique_ptr<const expression>> expressions;
    };

    class statement
    {
    public:
      statement() = default;
      statement(const statement &orig) = delete;
      virtual ~statement() = default;
    };

    class local_field_statement : public statement
    {
    public:
      local_field_statement(const std::vector<id_token> &ft, const std::vector<id_token> &ns, std::vector<std::unique_ptr<const expression>> es) : field_type(ft), names(ns), xprs(std::move(es)) {}
      local_field_statement(const local_field_statement &orig) = delete;
      virtual ~local_field_statement() = default;

    protected:
      const std::vector<id_token> field_type;
      const std::vector<id_token> names;
      const std::vector<std::unique_ptr<const expression>> xprs;
    };

    class assignment_statement : public statement
    {
    public:
      assignment_statement(const std::vector<id_token> &is, const id_token &i, const expression *const e) : ids(is), id(i), xpr(std::move(e)) {}
      assignment_statement(const assignment_statement &orig) = delete;
      virtual ~assignment_statement() = default;

    protected:
      const std::vector<id_token> ids;
      const id_token id;
      const expression *const xpr;
    };

    class expression_statement : public statement
    {
    public:
      RIDDLE_EXPORT expression_statement(const expression *const e) : xpr(std::move(e)) {}
      expression_statement(const expression_statement &orig) = delete;
      RIDDLE_EXPORT virtual ~expression_statement() = default;

    protected:
      const expression *const xpr;
    };

    class disjunction_statement : public statement
    {
    public:
      disjunction_statement(std::vector<std::vector<std::unique_ptr<const ast::statement>>> conjs, std::vector<std::unique_ptr<const expression>> conj_costs) : conjunctions(std::move(conjs)), conjunction_costs(std::move(conj_costs)) {}
      disjunction_statement(const disjunction_statement &orig) = delete;
      virtual ~disjunction_statement() = default;

    protected:
      const std::vector<std::vector<std::unique_ptr<const ast::statement>>> conjunctions;
      const std::vector<std::unique_ptr<const expression>> conjunction_costs;
    };

    class conjunction_statement : public statement
    {
    public:
      conjunction_statement(std::vector<std::unique_ptr<const ast::statement>> stmnts) : statements(std::move(stmnts)) {}
      conjunction_statement(const conjunction_statement &orig) = delete;
      virtual ~conjunction_statement() = default;

    protected:
      const std::vector<std::unique_ptr<const ast::statement>> statements;
    };

    class formula_statement : public statement
    {
    public:
      formula_statement(const bool &isf, const id_token &fn, const std::vector<id_token> &scp, const id_token &pn, const std::vector<id_token> &assn_ns, std::vector<std::unique_ptr<const expression>> assn_vs) : is_fact(isf), formula_name(fn), formula_scope(scp), predicate_name(pn), assignment_names(assn_ns), assignment_values(std::move(assn_vs)) {}
      formula_statement(const formula_statement &orig) = delete;
      virtual ~formula_statement() = default;

    protected:
      const bool is_fact;
      const id_token formula_name;
      const std::vector<id_token> formula_scope;
      const id_token predicate_name;
      const std::vector<id_token> assignment_names;
      const std::vector<std::unique_ptr<const expression>> assignment_values;
    };

    class return_statement : public statement
    {
    public:
      return_statement(const expression *const e) : xpr(std::move(e)) {}
      return_statement(const return_statement &orig) = delete;
      virtual ~return_statement() = default;

    protected:
      const expression *const xpr;
    };

    class type_declaration
    {
    public:
      type_declaration() {}
      type_declaration(const type_declaration &orig) = delete;
      virtual ~type_declaration() {}
    };

    class method_declaration
    {
    public:
      method_declaration(const std::vector<id_token> &rt, const id_token &n, const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, std::vector<std::unique_ptr<const ast::statement>> stmnts) : return_type(rt), name(n), parameters(pars), statements(std::move(stmnts)) {}
      method_declaration(const method_declaration &orig) = delete;
      virtual ~method_declaration() = default;

    protected:
      const std::vector<id_token> return_type;
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<std::unique_ptr<const ast::statement>> statements;
    };

    class predicate_declaration
    {
    public:
      predicate_declaration(const id_token &n, const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, const std::vector<std::vector<id_token>> &pl, std::vector<std::unique_ptr<const ast::statement>> stmnts) : name(n), parameters(pars), predicate_list(pl), statements(std::move(stmnts)) {}
      predicate_declaration(const predicate_declaration &orig) = delete;
      virtual ~predicate_declaration() = default;

    protected:
      const id_token name;
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<std::vector<id_token>> predicate_list;
      const std::vector<std::unique_ptr<const ast::statement>> statements;
    };

    class typedef_declaration : public type_declaration
    {
    public:
      typedef_declaration(const id_token &n, const id_token &pt, const expression *const e) : name(n), primitive_type(pt), xpr(std::move(e)) {}
      typedef_declaration(const typedef_declaration &orig) = delete;
      virtual ~typedef_declaration() = default;

    protected:
      const id_token name;
      const id_token primitive_type;
      const expression *const xpr;
    };

    class enum_declaration : public type_declaration
    {
    public:
      enum_declaration(const id_token &n, const std::vector<string_token> &es, const std::vector<std::vector<id_token>> &trs) : name(n), enums(es), type_refs(trs) {}
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
      variable_declaration(const id_token &n, const expression *const e = nullptr) : name(n), xpr(std::move(e)) {}
      variable_declaration(const variable_declaration &orig) = delete;
      virtual ~variable_declaration() = default;

    protected:
      const id_token name;
      const expression *const xpr;
    };

    class field_declaration
    {
    public:
      field_declaration(const std::vector<id_token> &tp, std::vector<std::unique_ptr<const variable_declaration>> ds) : field_type(tp), declarations(std::move(ds)) {}
      field_declaration(const field_declaration &orig) = delete;
      virtual ~field_declaration() = default;

    protected:
      const std::vector<id_token> field_type;
      const std::vector<std::unique_ptr<const variable_declaration>> declarations;
    };

    class constructor_declaration
    {
    public:
      constructor_declaration(const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, const std::vector<id_token> &ins, std::vector<std::vector<std::unique_ptr<const expression>>> ivs, std::vector<std::unique_ptr<const ast::statement>> stmnts) : parameters(pars), init_names(ins), init_vals(std::move(ivs)), statements(std::move(stmnts)) {}
      constructor_declaration(const constructor_declaration &orig) = delete;
      virtual ~constructor_declaration() = default;

    protected:
      const std::vector<std::pair<const std::vector<id_token>, const id_token>> parameters;
      const std::vector<id_token> init_names;
      const std::vector<std::vector<std::unique_ptr<const expression>>> init_vals;
      const std::vector<std::unique_ptr<const ast::statement>> statements;
    };

    class class_declaration : public type_declaration
    {
    public:
      class_declaration(const id_token &n, const std::vector<std::vector<id_token>> &bcs, std::vector<std::unique_ptr<const field_declaration>> fs, std::vector<std::unique_ptr<const constructor_declaration>> cs, std::vector<std::unique_ptr<const method_declaration>> ms, std::vector<std::unique_ptr<const predicate_declaration>> ps, std::vector<std::unique_ptr<const type_declaration>> ts) : name(n), base_classes(bcs), fields(std::move(fs)), constructors(std::move(cs)), methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)) {}
      class_declaration(const class_declaration &orig) = delete;
      virtual ~class_declaration() = default;

    protected:
      const id_token name;
      const std::vector<std::vector<id_token>> base_classes;
      const std::vector<std::unique_ptr<const field_declaration>> fields;
      const std::vector<std::unique_ptr<const constructor_declaration>> constructors;
      const std::vector<std::unique_ptr<const method_declaration>> methods;
      const std::vector<std::unique_ptr<const predicate_declaration>> predicates;
      const std::vector<std::unique_ptr<const type_declaration>> types;
    };

    class compilation_unit
    {
    public:
      compilation_unit(std::vector<std::unique_ptr<const method_declaration>> ms, std::vector<std::unique_ptr<const predicate_declaration>> ps, std::vector<std::unique_ptr<const type_declaration>> ts, std::vector<std::unique_ptr<const ast::statement>> stmnts) : methods(std::move(ms)), predicates(std::move(ps)), types(std::move(ts)), statements(std::move(stmnts)) {}
      compilation_unit(const compilation_unit &orig) = delete;
      virtual ~compilation_unit() = default;

    protected:
      const std::vector<std::unique_ptr<const method_declaration>> methods;
      const std::vector<std::unique_ptr<const predicate_declaration>> predicates;
      const std::vector<std::unique_ptr<const type_declaration>> types;
      const std::vector<std::unique_ptr<const ast::statement>> statements;
    };
  } // namespace ast

  class parser
  {
  public:
    RIDDLE_EXPORT parser(std::istream &is);
    parser(const parser &orig) = delete;
    RIDDLE_EXPORT virtual ~parser();

    RIDDLE_EXPORT std::unique_ptr<const ast::compilation_unit> parse();

  private:
    token *next();
    bool match(const symbol &sym);
    void backtrack(const size_t &p) noexcept;

    std::unique_ptr<const ast::typedef_declaration> _typedef_declaration();
    std::unique_ptr<const ast::enum_declaration> _enum_declaration();
    std::unique_ptr<const ast::class_declaration> _class_declaration();
    std::unique_ptr<const ast::field_declaration> _field_declaration();
    std::unique_ptr<const ast::method_declaration> _method_declaration();
    std::unique_ptr<const ast::constructor_declaration> _constructor_declaration();
    std::unique_ptr<const ast::predicate_declaration> _predicate_declaration();
    std::unique_ptr<const ast::statement> _statement();
    std::unique_ptr<const ast::expression> _expression(const size_t &pr = 0);

    void error(const std::string &err);

    /**
     * The declarations.
     */
    virtual std::unique_ptr<const ast::method_declaration> new_method_declaration(const std::vector<id_token> &rt, const id_token &n, const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, std::vector<std::unique_ptr<const ast::statement>> stmnts) const noexcept { return std::make_unique<const ast::method_declaration>(rt, n, pars, std::move(stmnts)); }
    virtual std::unique_ptr<const ast::predicate_declaration> new_predicate_declaration(const id_token &n, const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, const std::vector<std::vector<id_token>> &pl, std::vector<std::unique_ptr<const ast::statement>> stmnts) const noexcept { return std::make_unique<const ast::predicate_declaration>(n, pars, pl, std::move(stmnts)); }
    virtual std::unique_ptr<const ast::typedef_declaration> new_typedef_declaration(const id_token &n, const id_token &pt, const ast::expression *e) const noexcept { return std::make_unique<const ast::typedef_declaration>(n, pt, e); }
    virtual std::unique_ptr<const ast::enum_declaration> new_enum_declaration(const id_token &n, const std::vector<string_token> &es, const std::vector<std::vector<id_token>> &trs) const noexcept { return std::make_unique<const ast::enum_declaration>(n, es, trs); }
    virtual std::unique_ptr<const ast::class_declaration> new_class_declaration(const id_token &n, const std::vector<std::vector<id_token>> &bcs, std::vector<std::unique_ptr<const ast::field_declaration>> fs, std::vector<std::unique_ptr<const ast::constructor_declaration>> cs, std::vector<std::unique_ptr<const ast::method_declaration>> ms, std::vector<std::unique_ptr<const ast::predicate_declaration>> ps, std::vector<std::unique_ptr<const ast::type_declaration>> ts) const noexcept { return std::make_unique<const ast::class_declaration>(n, bcs, std::move(fs), std::move(cs), std::move(ms), std::move(ps), std::move(ts)); }
    virtual std::unique_ptr<const ast::variable_declaration> new_variable_declaration(const id_token &n, const ast::expression *const e = nullptr) const noexcept { return std::make_unique<const ast::variable_declaration>(n, e); }
    virtual std::unique_ptr<const ast::field_declaration> new_field_declaration(const std::vector<id_token> &tp, std::vector<std::unique_ptr<const ast::variable_declaration>> ds) const noexcept { return std::make_unique<const ast::field_declaration>(tp, std::move(ds)); }
    virtual std::unique_ptr<const ast::constructor_declaration> new_constructor_declaration(const std::vector<std::pair<const std::vector<id_token>, const id_token>> &pars, const std::vector<id_token> &ins, std::vector<std::vector<std::unique_ptr<const ast::expression>>> ivs, std::vector<std::unique_ptr<const ast::statement>> stmnts) const noexcept { return std::make_unique<const ast::constructor_declaration>(pars, ins, std::move(ivs), std::move(stmnts)); }
    virtual std::unique_ptr<const ast::compilation_unit> new_compilation_unit(std::vector<std::unique_ptr<const ast::method_declaration>> ms, std::vector<std::unique_ptr<const ast::predicate_declaration>> ps, std::vector<std::unique_ptr<const ast::type_declaration>> ts, std::vector<std::unique_ptr<const ast::statement>> stmnts) const noexcept { return std::make_unique<const ast::compilation_unit>(std::move(ms), std::move(ps), std::move(ts), std::move(stmnts)); }

    /**
     * The statements.
     */
    virtual std::unique_ptr<const ast::local_field_statement> new_local_field_statement(const std::vector<id_token> &ft, const std::vector<id_token> &ns, std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::local_field_statement>(ft, ns, std::move(es)); }
    virtual std::unique_ptr<const ast::assignment_statement> new_assignment_statement(const std::vector<id_token> &is, const id_token &i, const ast::expression *const e) const noexcept { return std::make_unique<const ast::assignment_statement>(is, i, e); }
    virtual std::unique_ptr<const ast::expression_statement> new_expression_statement(const ast::expression *const e) const noexcept { return std::make_unique<const ast::expression_statement>(e); }
    virtual std::unique_ptr<const ast::disjunction_statement> new_disjunction_statement(std::vector<std::vector<std::unique_ptr<const ast::statement>>> conjs, std::vector<std::unique_ptr<const ast::expression>> conj_costs) const noexcept { return std::make_unique<const ast::disjunction_statement>(std::move(conjs), std::move(conj_costs)); }
    virtual std::unique_ptr<const ast::conjunction_statement> new_conjunction_statement(std::vector<std::unique_ptr<const ast::statement>> stmnts) const noexcept { return std::make_unique<const ast::conjunction_statement>(std::move(stmnts)); }
    virtual std::unique_ptr<const ast::formula_statement> new_formula_statement(const bool &isf, const id_token &fn, const std::vector<id_token> &scp, const id_token &pn, const std::vector<id_token> &assn_ns, std::vector<std::unique_ptr<const ast::expression>> assn_vs) const noexcept { return std::make_unique<const ast::formula_statement>(isf, fn, scp, pn, assn_ns, std::move(assn_vs)); }
    virtual std::unique_ptr<const ast::return_statement> new_return_statement(const ast::expression *const e) const noexcept { return std::make_unique<const ast::return_statement>(e); }

    /**
     * The expressions.
     */
    virtual std::unique_ptr<const ast::bool_literal_expression> new_bool_literal_expression(const bool_token &l) const noexcept { return std::make_unique<const ast::bool_literal_expression>(l); }
    virtual std::unique_ptr<const ast::int_literal_expression> new_int_literal_expression(const int_token &l) const noexcept { return std::make_unique<const ast::int_literal_expression>(l); }
    virtual std::unique_ptr<const ast::real_literal_expression> new_real_literal_expression(const real_token &l) const noexcept { return std::make_unique<const ast::real_literal_expression>(l); }
    virtual std::unique_ptr<const ast::string_literal_expression> new_string_literal_expression(const string_token &l) const noexcept { return std::make_unique<const ast::string_literal_expression>(l); }
    virtual std::unique_ptr<const ast::cast_expression> new_cast_expression(const std::vector<id_token> &tp, std::unique_ptr<const ast::expression> e) const noexcept { return std::make_unique<const ast::cast_expression>(tp, std::move(e)); }
    virtual std::unique_ptr<const ast::plus_expression> new_plus_expression(std::unique_ptr<const ast::expression> e) const noexcept { return std::make_unique<const ast::plus_expression>(std::move(e)); }
    virtual std::unique_ptr<const ast::minus_expression> new_minus_expression(std::unique_ptr<const ast::expression> e) const noexcept { return std::make_unique<const ast::minus_expression>(std::move(e)); }
    virtual std::unique_ptr<const ast::not_expression> new_not_expression(std::unique_ptr<const ast::expression> e) const noexcept { return std::make_unique<const ast::not_expression>(std::move(e)); }
    virtual std::unique_ptr<const ast::constructor_expression> new_constructor_expression(const std::vector<id_token> &it, std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::constructor_expression>(it, std::move(es)); }
    virtual std::unique_ptr<const ast::eq_expression> new_eq_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::eq_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::neq_expression> new_neq_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::neq_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::lt_expression> new_lt_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::lt_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::leq_expression> new_leq_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::leq_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::geq_expression> new_geq_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::geq_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::gt_expression> new_gt_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::gt_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::function_expression> new_function_expression(const std::vector<id_token> &is, const id_token &fn, std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::function_expression>(is, fn, std::move(es)); }
    virtual std::unique_ptr<const ast::id_expression> new_id_expression(const std::vector<id_token> &is) const noexcept { return std::make_unique<const ast::id_expression>(is); }
    virtual std::unique_ptr<const ast::implication_expression> new_implication_expression(std::unique_ptr<const ast::expression> l, std::unique_ptr<const ast::expression> r) const noexcept { return std::make_unique<const ast::implication_expression>(std::move(l), std::move(r)); }
    virtual std::unique_ptr<const ast::disjunction_expression> new_disjunction_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::disjunction_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::conjunction_expression> new_conjunction_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::conjunction_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::exct_one_expression> new_exct_one_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::exct_one_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::addition_expression> new_addition_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::addition_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::subtraction_expression> new_subtraction_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::subtraction_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::multiplication_expression> new_multiplication_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::multiplication_expression>(std::move(es)); }
    virtual std::unique_ptr<const ast::division_expression> new_division_expression(std::vector<std::unique_ptr<const ast::expression>> es) const noexcept { return std::make_unique<const ast::division_expression>(std::move(es)); }

  private:
    lexer lex;                // the current lexer..
    token *tk = nullptr;      // the current lookahead token..
    std::vector<token *> tks; // all the tokens parsed so far..
    size_t pos = 0;           // the current position within tks'..
  };
} // namespace riddle