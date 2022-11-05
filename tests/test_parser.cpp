#include "riddle_parser.h"
#include <cassert>

using namespace riddle;

void test_parser_0()
{
    parser prs("real a;\n1 <= a;");
    auto cu = prs.parse();
}

void test_parser_1()
{
    parser prs("real a = 5 +2;\nfalse;");
    auto cu = prs.parse();
}

void test_parser_2()
{
    parser prs("goal g0 = new At(l:5+3);");
    auto cu = prs.parse();
}

void test_parser_3()
{
    parser prs("enum Speed {\"High\", \"Medium\", \"Low\"}; Speed x3;");
    auto cu = prs.parse();
}

int main(int, char **)
{
    test_parser_0();
    test_parser_1();
    test_parser_2();
    test_parser_3();
}