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
    std::shared_ptr<riddle::arith_item> new_real() override { return core::new_real(utils::rational(0)); }
    std::shared_ptr<riddle::string_item> new_string() override { return core::new_string(""); }
};

void test_enum_declaration()
{
    test_core core;
    core.read("enum E { \"a\", \"b\", \"c\" } | a.b.c;");
}

int main()
{
    test_enum_declaration();
    return 0;
}
