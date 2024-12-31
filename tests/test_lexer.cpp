#include "lexer.hpp"
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

void test_lexer3()
{
    riddle::lexer lex;
    std::stringstream ss;
    ss << "string a = \"hello\";";
    auto tokens = lex.parse(ss);
    assert(tokens.size() == 6);
    assert(tokens[0]->sym == riddle::STRING);
    assert(tokens[0]->line == 1);
    assert(tokens[0]->start_pos == 0);
    assert(tokens[0]->end_pos == 5);
    assert(tokens[1]->sym == riddle::ID);
    assert(tokens[1]->line == 1);
    assert(tokens[1]->start_pos == 7);
    assert(tokens[1]->end_pos == 7);
    assert(tokens[2]->sym == riddle::EQ);
    assert(tokens[2]->line == 1);
    assert(tokens[2]->start_pos == 9);
    assert(tokens[2]->end_pos == 9);
    assert(tokens[3]->sym == riddle::String);
    assert(tokens[3]->line == 1);
    assert(tokens[3]->start_pos == 11);
    assert(tokens[3]->end_pos == 17);
    assert(tokens[4]->sym == riddle::SEMICOLON);
    assert(tokens[4]->line == 1);
    assert(tokens[4]->start_pos == 18);
    assert(tokens[4]->end_pos == 18);
    assert(tokens[5]->sym == riddle::EoF);
}

void test_lexer4()
{
    riddle::lexer lex;
    std::stringstream ss;
    ss << "enum E { \"a\", \"b\", \"c\" } | a.b.c;";
    auto tokens = lex.parse(ss);
    assert(tokens.size() == 17);
    assert(tokens[0]->sym == riddle::ENUM);
    assert(tokens[0]->line == 1);
    assert(tokens[0]->start_pos == 0);
    assert(tokens[0]->end_pos == 3);
    assert(tokens[1]->sym == riddle::ID);
    assert(tokens[1]->line == 1);
    assert(tokens[1]->start_pos == 5);
    assert(tokens[1]->end_pos == 5);
    assert(tokens[2]->sym == riddle::LBRACE);
    assert(tokens[2]->line == 1);
    assert(tokens[2]->start_pos == 7);
    assert(tokens[2]->end_pos == 7);
    assert(tokens[3]->sym == riddle::String);
    assert(tokens[3]->line == 1);
    assert(tokens[3]->start_pos == 9);
    assert(tokens[3]->end_pos == 11);
    assert(tokens[4]->sym == riddle::COMMA);
    assert(tokens[4]->line == 1);
    assert(tokens[4]->start_pos == 12);
    assert(tokens[4]->end_pos == 12);
    assert(tokens[5]->sym == riddle::String);
    assert(tokens[5]->line == 1);
    assert(tokens[5]->start_pos == 14);
    assert(tokens[5]->end_pos == 16);
    assert(tokens[6]->sym == riddle::COMMA);
    assert(tokens[6]->line == 1);
    assert(tokens[6]->start_pos == 17);
    assert(tokens[6]->end_pos == 17);
    assert(tokens[7]->sym == riddle::String);
    assert(tokens[7]->line == 1);
    assert(tokens[7]->start_pos == 19);
    assert(tokens[7]->end_pos == 21);
    assert(tokens[8]->sym == riddle::RBRACE);
    assert(tokens[8]->line == 1);
    assert(tokens[8]->start_pos == 23);
    assert(tokens[8]->end_pos == 23);
    assert(tokens[9]->sym == riddle::BAR);
    assert(tokens[9]->line == 1);
    assert(tokens[9]->start_pos == 25);
    assert(tokens[9]->end_pos == 25);
    assert(tokens[10]->sym == riddle::ID);
    assert(tokens[10]->line == 1);
    assert(tokens[10]->start_pos == 27);
    assert(tokens[10]->end_pos == 27);
    assert(tokens[11]->sym == riddle::DOT);
    assert(tokens[11]->line == 1);
    assert(tokens[11]->start_pos == 28);
    assert(tokens[11]->end_pos == 28);
    assert(tokens[12]->sym == riddle::ID);
    assert(tokens[12]->line == 1);
    assert(tokens[12]->start_pos == 29);
    assert(tokens[12]->end_pos == 29);
    assert(tokens[13]->sym == riddle::DOT);
    assert(tokens[13]->line == 1);
    assert(tokens[13]->start_pos == 30);
    assert(tokens[13]->end_pos == 30);
    assert(tokens[14]->sym == riddle::ID);
    assert(tokens[14]->line == 1);
    assert(tokens[14]->start_pos == 31);
    assert(tokens[14]->end_pos == 31);
    assert(tokens[15]->sym == riddle::SEMICOLON);
    assert(tokens[15]->line == 1);
    assert(tokens[15]->start_pos == 32);
    assert(tokens[15]->end_pos == 32);
    assert(tokens[16]->sym == riddle::EoF);
}

int main()
{
    test_lexer0();
    test_lexer1();
    test_lexer2();
    test_lexer3();
    test_lexer4();
    return 0;
}
