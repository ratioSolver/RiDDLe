#include "parser.hpp"
#include <cassert>

void test_enum_declaration()
{
    riddle::parser parser("enum E { \"a\", \"b\", \"c\" } | a.b.c;");
    auto enum_decl = parser.parse_enum_declaration();
}

int main()
{
    test_enum_declaration();
    return 0;
}
