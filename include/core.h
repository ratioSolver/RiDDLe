#pragma once

#include "type.h"
#include "item.h"
#include "method.h"

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

    virtual expr new_string() = 0;
    virtual expr new_string(const std::string &value) = 0;

    virtual expr add(const std::vector<expr> &exprs) = 0;
    virtual expr sub(const std::vector<expr> &exprs) = 0;
    virtual expr mul(const std::vector<expr> &exprs) = 0;
    virtual expr div(const std::vector<expr> &exprs) = 0;
    virtual expr minus(const expr &exprs) = 0;

    virtual expr lt(const expr &lhs, const expr &rhs) = 0;
    virtual expr leq(const expr &lhs, const expr &rhs) = 0;
    virtual expr gt(const expr &lhs, const expr &rhs) = 0;
    virtual expr geq(const expr &lhs, const expr &rhs) = 0;
    virtual expr eq(const expr &lhs, const expr &rhs) = 0;

    virtual expr conj(const std::vector<expr> &exprs) = 0;
    virtual expr disj(const std::vector<expr> &exprs) = 0;
    virtual expr exct_one(const std::vector<expr> &exprs) = 0;
    virtual expr negate(const expr &expr) = 0;

    virtual expr new_fact(const predicate &pred) = 0;
    virtual expr new_goal(const predicate &pred) = 0;

    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;

    RIDDLE_EXPORT expr &get(const std::string &name) override;

  private:
    std::map<std::string, type_ptr> types;
    std::map<std::string, std::vector<method_ptr>> methods;
  };
} // namespace riddle