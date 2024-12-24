#include "parser.hpp"
#include "core.hpp"
#include <cassert>

class test_core : public riddle::core
{
public:
    test_core() noexcept : riddle::core() {}
    ~test_core() override = default;

    std::shared_ptr<riddle::bool_item> new_bool() override { return core::new_bool(false); }
    std::shared_ptr<riddle::arith_item> new_int() override { return core::new_int(0); }
    std::shared_ptr<riddle::arith_item> new_int(const INT_TYPE lb, const INT_TYPE ub) override { return core::new_int(lb); }
    std::shared_ptr<riddle::arith_item> new_uncertain_int(const INT_TYPE lb, const INT_TYPE ub) override { return core::new_int(lb); }
    std::shared_ptr<riddle::arith_item> new_real() override { return core::new_real(utils::rational(0)); }
    std::shared_ptr<riddle::arith_item> new_real(utils::rational &&lb, utils::rational &&ub) override { return core::new_real(utils::rational(lb)); }
    std::shared_ptr<riddle::arith_item> new_uncertain_real(utils::rational &&lb, utils::rational &&ub) override { return core::new_real(utils::rational(lb)); }
    std::shared_ptr<riddle::arith_item> new_time() override { return core::new_time(utils::rational(0)); }
    std::shared_ptr<riddle::string_item> new_string() override { return core::new_string(""); }
    std::shared_ptr<riddle::enum_item> new_enum(riddle::type &tp, std::vector<std::reference_wrapper<utils::enum_val>> &&values) override { return std::make_shared<riddle::enum_item>(tp, utils::var()); }

    std::shared_ptr<riddle::bool_item> new_and(std::vector<std::shared_ptr<riddle::bool_item>> &&exprs) override { return core::new_bool(false); }
    std::shared_ptr<riddle::bool_item> new_or(std::vector<std::shared_ptr<riddle::bool_item>> &&exprs) override { return core::new_bool(false); }
    std::shared_ptr<riddle::bool_item> new_xor(std::vector<std::shared_ptr<riddle::bool_item>> &&exprs) override { return core::new_bool(false); }

    std::shared_ptr<riddle::bool_item> new_not(std::shared_ptr<riddle::bool_item> expr) override { return core::new_bool(false); }

    std::shared_ptr<riddle::arith_item> new_negation(std::shared_ptr<riddle::arith_item> xpr) override { return core::new_int(0); }

    std::shared_ptr<riddle::arith_item> new_sum(std::vector<std::shared_ptr<riddle::arith_item>> &&xprs) override { return core::new_int(0); }
    std::shared_ptr<riddle::arith_item> new_product(std::vector<std::shared_ptr<riddle::arith_item>> &&xprs) override { return core::new_int(0); }
    std::shared_ptr<riddle::arith_item> new_divide(std::shared_ptr<riddle::arith_item> lhs, std::shared_ptr<riddle::arith_item> rhs) override { return core::new_int(0); }

    std::shared_ptr<riddle::bool_item> new_lt(std::shared_ptr<riddle::arith_item> lhs, std::shared_ptr<riddle::arith_item> rhs) override { return core::new_bool(false); }
    std::shared_ptr<riddle::bool_item> new_le(std::shared_ptr<riddle::arith_item> lhs, std::shared_ptr<riddle::arith_item> rhs) override { return core::new_bool(false); }
    std::shared_ptr<riddle::bool_item> new_gt(std::shared_ptr<riddle::arith_item> lhs, std::shared_ptr<riddle::arith_item> rhs) override { return core::new_bool(false); }
    std::shared_ptr<riddle::bool_item> new_ge(std::shared_ptr<riddle::arith_item> lhs, std::shared_ptr<riddle::arith_item> rhs) override { return core::new_bool(false); }

    std::shared_ptr<riddle::bool_item> new_eq(std::shared_ptr<riddle::item> lhs, std::shared_ptr<riddle::item> rhs) override { return core::new_bool(false); }
};

void test_class_declaration()
{
    test_core core;
    core.read("class A { int a; };");
}

void test_field_declaration()
{
    test_core core;
    core.read("class A { int a = 0; };");
}

void test_method_declaration()
{
    test_core core;
    core.read("class A { int a() { return 0; } };");
}

void test_enum_declaration()
{
    test_core core;
    core.read("enum E { \"a\", \"b\", \"c\" };");
}

int main()
{
    test_class_declaration();
    test_field_declaration();
    test_method_declaration();
    test_enum_declaration();
    return 0;
}
