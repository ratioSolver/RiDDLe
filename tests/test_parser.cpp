#include "core.hpp"
#include "lit.hpp"
#include "lin.hpp"
#include <cassert>

class bool_item : public riddle::bool_item
{
public:
    bool_item(riddle::bool_type &tp, utils::lit &&expr) : riddle::bool_item(tp), expr(expr) {}

    [[nodiscard]] utils::lit &get_expr() noexcept { return expr; }
    [[nodiscard]] const utils::lit &get_expr() const noexcept { return expr; }

    [[nodiscard]] riddle::bool_expr operator==(riddle::expr) const override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type()), std::move(f));
    }

private:
    utils::lit expr;
};

class arith_item : public riddle::arith_item
{
public:
    arith_item(riddle::int_type &tp, utils::lin &&expr) : riddle::arith_item(tp), expr(expr) {}
    arith_item(riddle::real_type &tp, utils::lin &&expr) : riddle::arith_item(tp), expr(expr) {}
    arith_item(riddle::time_type &tp, utils::lin &&expr) : riddle::arith_item(tp), expr(expr) {}

    [[nodiscard]] utils::lin &get_expr() noexcept { return expr; }
    [[nodiscard]] const utils::lin &get_expr() const noexcept { return expr; }

    [[nodiscard]] riddle::bool_expr operator==(riddle::expr) const override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type()), std::move(f));
    }

private:
    utils::lin expr;
};

class string_item : public riddle::string_item
{
public:
    string_item(riddle::string_type &tp, std::string &&expr) : riddle::string_item(tp), expr(expr) {}

    [[nodiscard]] std::string &get_expr() noexcept { return expr; }
    [[nodiscard]] const std::string &get_expr() const noexcept { return expr; }

    [[nodiscard]] riddle::bool_expr operator==(riddle::expr) const override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type()), std::move(f));
    }

private:
    std::string expr;
};

class enum_item : public riddle::enum_item
{
public:
    enum_item(riddle::type &tp, utils::var &&expr, std::vector<utils::ref_wrapper<utils::enum_val>> &&values) : riddle::enum_item(tp, std::move(values)), expr(expr) {}

    [[nodiscard]] utils::var &get_expr() noexcept { return expr; }
    [[nodiscard]] const utils::var &get_expr() const noexcept { return expr; }

    [[nodiscard]] riddle::bool_expr operator==(riddle::expr) const override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type()), std::move(f));
    }

private:
    utils::var expr;
};

class atom : public riddle::atom
{
public:
    atom(riddle::predicate &pred, bool is_fact, utils::lit &&expr, std::map<std::string, utils::s_ptr<riddle::item>, std::less<>> &&args) : riddle::atom(pred, is_fact, std::move(args)), expr(expr) {}

    [[nodiscard]] utils::lit &get_sigma() noexcept { return expr; }
    [[nodiscard]] const utils::lit &get_sigma() const noexcept { return expr; }

    [[nodiscard]] riddle::bool_expr operator==(riddle::expr) const override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type()), std::move(f));
    }

private:
    utils::lit expr;
};

class test_core : public riddle::core
{
public:
    test_core() noexcept : riddle::core() {}
    ~test_core() override = default;

    riddle::bool_expr new_bool(const bool) override { return utils::make_s_ptr<bool_item>(static_cast<riddle::bool_type &>(get_type(riddle::bool_kw)), utils::lit()); }
    riddle::bool_expr new_bool() override { return new_bool(false); }
    utils::lbool bool_value(const riddle::bool_item &) const noexcept override { return utils::Undefined; }
    riddle::arith_expr new_int(const INT_TYPE) override { return utils::make_s_ptr<arith_item>(static_cast<riddle::int_type &>(get_type(riddle::int_kw)), utils::lin()); }
    riddle::arith_expr new_int() override { return new_int(0); }
    riddle::arith_expr new_int(const INT_TYPE lb, const INT_TYPE) override { return new_int(lb); }
    riddle::arith_expr new_uncertain_int(const INT_TYPE lb, const INT_TYPE) override { return new_int(lb); }
    riddle::arith_expr new_real(utils::rational &&) override { return utils::make_s_ptr<arith_item>(static_cast<riddle::real_type &>(get_type(riddle::real_kw)), utils::lin()); }
    riddle::arith_expr new_real() override { return new_real(utils::rational(0)); }
    riddle::arith_expr new_real(utils::rational &&lb, utils::rational &&) override { return new_real(utils::rational(lb)); }
    riddle::arith_expr new_uncertain_real(utils::rational &&lb, utils::rational &&) override { return new_real(utils::rational(lb)); }
    riddle::arith_expr new_time(utils::rational &&) override { return utils::make_s_ptr<arith_item>(static_cast<riddle::time_type &>(get_type(riddle::time_kw)), utils::lin()); }
    riddle::arith_expr new_time() override { return new_time(utils::rational(0)); }
    utils::inf_rational arith_value(const riddle::arith_item &) const noexcept override { return utils::inf_rational(); }
    riddle::string_expr new_string(std::string &&) override { return utils::make_s_ptr<string_item>(static_cast<riddle::string_type &>(get_type(riddle::string_kw)), ""); }
    riddle::string_expr new_string() override { return new_string(""); }
    std::string string_value(const riddle::string_item &) const noexcept override { return ""; }
    riddle::enum_expr new_enum(riddle::type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values) override { return utils::make_s_ptr<enum_item>(tp, 0, std::move(values)); }
    std::vector<utils::ref_wrapper<utils::enum_val>> enum_value(const riddle::enum_item &itm) const noexcept override { return {*itm.get_values()[0]}; }

    riddle::bool_expr new_and(std::vector<riddle::bool_expr> &&) override { return new_bool(false); }
    riddle::bool_expr new_or(std::vector<riddle::bool_expr> &&) override { return new_bool(false); }
    riddle::bool_expr new_xor(std::vector<riddle::bool_expr> &&) override { return new_bool(false); }

    riddle::bool_expr new_not(riddle::bool_expr) override { return new_bool(false); }

    riddle::arith_expr new_negation(riddle::arith_expr) override { return new_int(0); }

    riddle::arith_expr new_sum(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_subtraction(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_product(std::vector<riddle::arith_expr> &&) override { return new_int(0); }
    riddle::arith_expr new_division(std::vector<riddle::arith_expr> &&) override { return new_int(0); }

    riddle::bool_expr new_lt(riddle::arith_expr, riddle::arith_expr) override { return new_bool(false); }
    riddle::bool_expr new_le(riddle::arith_expr, riddle::arith_expr) override { return new_bool(false); }
    riddle::bool_expr new_gt(riddle::arith_expr, riddle::arith_expr) override { return new_bool(false); }
    riddle::bool_expr new_ge(riddle::arith_expr, riddle::arith_expr) override { return new_bool(false); }

    void new_disjunction(std::vector<utils::u_ptr<riddle::conjunction>> &&) override {}
    void assert_clause(std::vector<riddle::bool_expr> &&) override {}

    riddle::atom_expr create_atom(bool is_fact, riddle::predicate &pred, std::map<std::string, utils::s_ptr<riddle::item>, std::less<>> &&args) override
    {
        auto f = utils::FALSE_lit;
        return utils::make_s_ptr<atom>(pred, is_fact, std::move(f), std::move(args));
    }
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
