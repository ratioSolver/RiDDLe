#pragma once

#include "type.h"
#include "method.h"
#include "inf_rational.h"

namespace riddle
{
  class core : public scope, public env
  {
  public:
    RIDDLE_EXPORT core();
    virtual ~core() = default;

    core &get_core() override { return *this; }

    virtual expr new_bool() = 0;
    virtual expr new_bool(bool value) = 0;

    virtual expr new_int() = 0;
    virtual expr new_int(utils::I value) = 0;

    virtual expr new_real() = 0;
    virtual expr new_real(utils::rational value) = 0;

    virtual expr new_time_point() = 0;
    virtual expr new_time_point(utils::rational value) = 0;

    virtual expr new_string() = 0;
    virtual expr new_string(const std::string &value) = 0;

    virtual expr new_enum(const type &tp, const std::vector<expr> &xprs) = 0;

    virtual expr add(const std::vector<expr> &xprs) = 0;
    virtual expr sub(const std::vector<expr> &xprs) = 0;
    virtual expr mul(const std::vector<expr> &xprs) = 0;
    virtual expr div(const std::vector<expr> &xprs) = 0;
    virtual expr minus(const expr &xprs) = 0;

    virtual expr lt(const expr &lhs, const expr &rhs) = 0;
    virtual expr leq(const expr &lhs, const expr &rhs) = 0;
    virtual expr gt(const expr &lhs, const expr &rhs) = 0;
    virtual expr geq(const expr &lhs, const expr &rhs) = 0;
    virtual expr eq(const expr &lhs, const expr &rhs) = 0;

    virtual expr conj(const std::vector<expr> &xprs) = 0;
    virtual expr disj(const std::vector<expr> &xprs) = 0;
    virtual expr exct_one(const std::vector<expr> &xprs) = 0;
    virtual expr negate(const expr &xpr) = 0;

    virtual void assert_fact(const expr &xpr) = 0;

    virtual void new_disjunction(const std::vector<conjunction_ptr> &conjs) = 0;

    virtual expr new_fact(const predicate &pred) = 0;
    virtual expr new_goal(const predicate &pred) = 0;

    virtual utils::inf_rational arith_value(const expr &xpr) const = 0;

    virtual bool is_enum(const expr &xpr) const = 0;
    virtual std::vector<expr> domain(const expr &xpr) const = 0;
    virtual void prune(const expr &xpr, const expr &val) = 0;

    type &get_bool_type() const noexcept { return *bt; }
    type &get_int_type() const noexcept { return *it; }
    type &get_real_type() const noexcept { return *rt; }
    type &get_time_type() const noexcept { return *tt; }
    type &get_string_type() const noexcept { return *st; }

    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;
    RIDDLE_EXPORT predicate &get_predicate(const std::string &name) override;

    RIDDLE_EXPORT expr &get(const std::string &name) override;

  private:
    type *bt, *it, *rt, *tt, *st;               // builtin types..
    std::vector<ast::compilation_unit_ptr> cus; // the compilation units..

  private:
    std::map<std::string, type_ptr> types;
    std::map<std::string, std::vector<method_ptr>> methods;
    std::map<std::string, predicate_ptr> predicates;
  };

  inline bool is_core(const scope &scp) noexcept { return &scp == &scp.get_core(); }
} // namespace riddle