#include "test_lexer.hpp"
#include <sstream>
#include <fstream>
#include <cassert>

void test_lexer0()
{
    riddle::lexer lex;
    std::stringstream ss;
    ss << "bool a = true;";
    auto tokens = lex.parse(ss);
    assert(tokens.size() == 6);
    assert(tokens[0]->sym == riddle::BOOL);
    assert(tokens[0]->line == 1);
    assert(tokens[0]->start_pos == 0);
    assert(tokens[0]->end_pos == 3);
    assert(tokens[1]->sym == riddle::ID);
    assert(tokens[1]->line == 1);
    assert(tokens[1]->start_pos == 5);
    assert(tokens[1]->end_pos == 5);
    assert(tokens[2]->sym == riddle::EQ);
    assert(tokens[2]->line == 1);
    assert(tokens[2]->start_pos == 7);
    assert(tokens[2]->end_pos == 7);
    assert(tokens[3]->sym == riddle::Bool);
    assert(tokens[3]->line == 1);
    assert(tokens[3]->start_pos == 9);
    assert(tokens[3]->end_pos == 12);
    assert(tokens[4]->sym == riddle::SEMICOLON);
    assert(tokens[4]->line == 1);
    assert(tokens[4]->start_pos == 13);
    assert(tokens[4]->end_pos == 13);
    assert(tokens[5]->sym == riddle::EoF);
}

void test_lexer1()
{
    riddle::lexer lex;
    std::stringstream ss;
    ss << "int a = 42;";
    auto tokens = lex.parse(ss);
    assert(tokens.size() == 6);
    assert(tokens[0]->sym == riddle::INT);
    assert(tokens[0]->line == 1);
    assert(tokens[0]->start_pos == 0);
    assert(tokens[0]->end_pos == 2);
    assert(tokens[1]->sym == riddle::ID);
    assert(tokens[1]->line == 1);
    assert(tokens[1]->start_pos == 4);
    assert(tokens[1]->end_pos == 4);
    assert(tokens[2]->sym == riddle::EQ);
    assert(tokens[2]->line == 1);
    assert(tokens[2]->start_pos == 6);
    assert(tokens[2]->end_pos == 6);
    assert(tokens[3]->sym == riddle::Int);
    assert(tokens[3]->line == 1);
    assert(tokens[3]->start_pos == 8);
    assert(tokens[3]->end_pos == 9);
    assert(tokens[4]->sym == riddle::SEMICOLON);
    assert(tokens[4]->line == 1);
    assert(tokens[4]->start_pos == 10);
    assert(tokens[4]->end_pos == 10);
    assert(tokens[5]->sym == riddle::EoF);
}

void test_lexer2()
{
    riddle::lexer lex;
    std::stringstream ss;
    ss << "real a = 3.14;";
    auto tokens = lex.parse(ss);
    assert(tokens.size() == 6);
    assert(tokens[0]->sym == riddle::REAL);
    assert(tokens[0]->line == 1);
    assert(tokens[0]->start_pos == 0);
    assert(tokens[0]->end_pos == 3);
    assert(tokens[1]->sym == riddle::ID);
    assert(tokens[1]->line == 1);
    assert(tokens[1]->start_pos == 5);
    assert(tokens[1]->end_pos == 5);
    assert(tokens[2]->sym == riddle::EQ);
    assert(tokens[2]->line == 1);
    assert(tokens[2]->start_pos == 7);
    assert(tokens[2]->end_pos == 7);
    assert(tokens[3]->sym == riddle::Real);
    assert(tokens[3]->line == 1);
    assert(tokens[3]->start_pos == 9);
    assert(tokens[3]->end_pos == 12);
    assert(tokens[4]->sym == riddle::SEMICOLON);
    assert(tokens[4]->line == 1);
    assert(tokens[4]->start_pos == 13);
    assert(tokens[4]->end_pos == 13);
    assert(tokens[5]->sym == riddle::EoF);
}

int main(int argc, char const *argv[])
{
    test_lexer0();
    test_lexer1();
    test_lexer2();
    return 0;
}
