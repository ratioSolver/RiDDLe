#include "core.hpp"
#include "flaw.hpp"
#include "items.hpp"
#include <cassert>

class test_enum_flaw : public riddle::flaw
{
public:
    test_enum_flaw(riddle::core &cr, riddle::component_type &tp, std::vector<riddle::expr> &&vals) noexcept : riddle::flaw(cr, {}), itm(std::make_shared<riddle::enum_item>(*this, tp, std::move(vals), 0)) {}

    [[nodiscard]] riddle::enum_expr get_enum() const noexcept { return itm; }

    utils::rational get_estimated_cost() const noexcept override { return utils::rational(1); }

private:
    riddle::enum_expr itm;
};

class test_atom_flaw : public riddle::flaw
{
public:
    test_atom_flaw(riddle::core &cr, bool is_fact, riddle::predicate &pred, std::map<std::string, std::shared_ptr<riddle::term>, std::less<>> &&args) noexcept : riddle::flaw(cr, {}), atm(std::make_shared<riddle::atom>(*this, pred, is_fact, std::move(args), cr.new_bool())) {}

    [[nodiscard]] riddle::atom_expr get_atom() const noexcept { return atm; }

    utils::rational get_estimated_cost() const noexcept override { return utils::rational(1); }

private:
    riddle::atom_expr atm;
};

class test_core : public riddle::core
{
public:
    test_core() noexcept : riddle::core() {}
    ~test_core() override = default;

    riddle::bool_expr new_bool(const bool) override { return std::make_shared<riddle::bool_item>(static_cast<riddle::bool_type &>(get_type(riddle::bool_kw)), utils::lit()); }
    riddle::bool_expr new_bool() override { return new_bool(false); }
    utils::lbool bool_value(const riddle::bool_term &) const noexcept override { return utils::Undefined; }
    riddle::arith_expr new_int(const INT_TYPE) override { return std::make_shared<riddle::arith_item>(static_cast<riddle::int_type &>(get_type(riddle::int_kw)), utils::lin()); }
    riddle::arith_expr new_int() override { return new_int(0); }
    riddle::arith_expr new_int(const INT_TYPE lb, const INT_TYPE) override { return new_int(lb); }
    riddle::arith_expr new_uncertain_int(const INT_TYPE lb, const INT_TYPE) override { return new_int(lb); }
    riddle::arith_expr new_real(utils::rational &&) override { return std::make_shared<riddle::arith_item>(static_cast<riddle::real_type &>(get_type(riddle::real_kw)), utils::lin()); }
    riddle::arith_expr new_real() override { return new_real(utils::rational(0)); }
    riddle::arith_expr new_real(utils::rational &&lb, utils::rational &&) override { return new_real(utils::rational(lb)); }
    riddle::arith_expr new_uncertain_real(utils::rational &&lb, utils::rational &&) override { return new_real(utils::rational(lb)); }
    riddle::arith_expr new_time(utils::rational &&) override { return std::make_shared<riddle::arith_item>(static_cast<riddle::time_type &>(get_type(riddle::time_kw)), utils::lin()); }
    riddle::arith_expr new_time() override { return new_time(utils::rational(0)); }
    utils::inf_rational arith_value(const riddle::arith_term &) const noexcept override { return utils::inf_rational(); }
    bool is_constant(const riddle::arith_term &) const noexcept override { return true; }
    riddle::string_expr new_string(std::string &&) override { return std::make_shared<riddle::string_item>(static_cast<riddle::string_type &>(get_type(riddle::string_kw)), ""); }
    riddle::string_expr new_string() override { return new_string(""); }
    std::string string_value(const riddle::string_term &) const noexcept override { return ""; }
    riddle::expr new_enum(riddle::component_type &tp, std::vector<riddle::expr> &&values) override
    {
        auto flw = std::make_shared<test_enum_flaw>(*this, tp, std::move(values));
        flaws.emplace_back(flw);
        return flw->get_enum();
    }
    std::unordered_set<riddle::expr> enum_value(const riddle::enum_term &xpr) const noexcept override { return {xpr.get_values()[0]}; }

    riddle::arith_expr new_negation(riddle::arith_expr) override { return new_int(0); }

    riddle::arith_expr new_sum(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_subtraction(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_product(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_division(std::vector<riddle::arith_expr> &&) override { return new_int(0); }

    void new_disjunction(std::vector<std::unique_ptr<riddle::conjunction>> &&) override {}
    void new_clause(std::vector<riddle::bool_expr> &&) override {}

    riddle::atom_expr create_atom(bool is_fact, riddle::predicate &pred, std::map<std::string, std::shared_ptr<riddle::term>, std::less<>> &&args) override
    {
        auto flw = std::make_shared<test_atom_flaw>(*this, is_fact, pred, std::move(args));
        flaws.emplace_back(flw);
        return flw->get_atom();
    }
    riddle::atom_state get_atom_state(const riddle::atom_term &) const noexcept override { return riddle::atom_state::active; }

private:
    bool mk_assign(riddle::bool_expr, utils::lbool) noexcept { return true; }
    bool mk_eq(riddle::bool_expr, riddle::bool_expr) noexcept { return true; }
    bool mk_neq(riddle::bool_expr, riddle::bool_expr) noexcept { return true; }

    bool mk_lt(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }
    bool mk_le(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }
    bool mk_eq(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }
    bool mk_neq(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }
    bool mk_ge(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }
    bool mk_gt(riddle::arith_expr, riddle::arith_expr) noexcept { return true; }

    bool mk_assign(riddle::enum_expr, const utils::enum_val &) noexcept { return true; }
    bool mk_forbid(riddle::enum_expr, const utils::enum_val &) noexcept { return true; }
    bool mk_eq(riddle::enum_expr, riddle::enum_expr) noexcept { return true; }
    bool mk_neq(riddle::enum_expr, riddle::enum_expr) noexcept { return true; }

private:
    std::vector<std::shared_ptr<riddle::flaw>> flaws;
};

void test_class_declaration()
{
    test_core core;
    core.read("class A { int a; };");
}

void test_class_inheritance()
{
    test_core core;
    core.read("class A { int a; }; class B : A { int b; };");
}

void test_field_declaration()
{
    test_core core;
    core.read("class A { int a = 0; };");
}

void test_constructor_declaration()
{
    test_core core;
    core.read("class A { A() { } };");
}

void test_method_declaration()
{
    test_core core;
    core.read("class A { int a() { return 0; } };");
    core.read("A a = new A(); int i = a.a();");
}

void test_enum_declaration()
{
    test_core core;
    core.read("enum E { \"a\", \"b\", \"c\" };");
}

void test_predicate_declaration()
{
    test_core core;
    core.read("predicate p(int a, int b) { a < b; }");
}

void test_items()
{
    test_core core;
    core.read("bool b;");
    core.read("int i;");
    core.read("real r;");
    core.read("time t;");
    core.read("string s;");
    core.read("enum E { \"a\", \"b\", \"c\" };");
    core.read("E e;");
}

void test_bounded_ariths()
{
    test_core core;
    core.read("int i = [0, 10];");
    core.read("real r = [0.0, 10.0];");
    core.read("time t = [0, 10];");
}

void test_uncertain_ariths()
{
    test_core core;
    core.read("int i = ?[0, 10];");
    core.read("real r = ?[0.0, 10.0];");
    core.read("time t = ?[0, 10];");
}

void test_statements()
{
    test_core core;
    core.read("int a, b, c;");
    core.read("2*a + 3*b < 4*c;");
}

void test_fact()
{
    test_core core;
    core.read("predicate p(int a, int b) { a < b; }");
    core.read("fact f = new p();");
    core.read("goal g = new p();");

    core.read("class A { predicate p(int a, int b) { a < b; } };");
    core.read("A a = new A(); fact f = new a.p();");
}

int main()
{
    test_class_declaration();
    test_class_inheritance();
    test_field_declaration();
    test_constructor_declaration();
    test_method_declaration();
    test_enum_declaration();
    test_predicate_declaration();
    test_items();
    test_bounded_ariths();
    test_uncertain_ariths();
    test_statements();
    test_fact();
    return 0;
}
