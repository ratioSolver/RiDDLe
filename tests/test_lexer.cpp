#include "riddle_lexer.h"
#include <sstream>
#include <cassert>

using namespace riddle;

void test_lexer_0()
{
    std::stringstream ss("real a = 5 + 2;\nfalse;");
    lexer l(ss);
    auto t0 = l.next();
    assert(t0->sym == REAL_ID);
    auto t1 = l.next();
    assert(t1->sym == ID_ID);
    auto t2 = l.next();
    assert(t2->sym == EQ_ID);
    auto t3 = l.next();
    assert(t3->sym == IntLiteral_ID);
    auto t4 = l.next();
    assert(t4->sym == PLUS_ID);
    auto t5 = l.next();
    assert(t5->sym == IntLiteral_ID);
    auto t6 = l.next();
    assert(t6->sym == SEMICOLON_ID);
    auto t7 = l.next();
    assert(t7->sym == BoolLiteral_ID);
    auto t8 = l.next();
    assert(t8->sym == SEMICOLON_ID);
    auto t9 = l.next();
    assert(t9->sym == EOF_ID);
}

int main(int, char **)
{
    test_lexer_0();
}