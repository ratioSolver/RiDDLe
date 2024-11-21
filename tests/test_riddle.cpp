#include <fstream>
#include <cassert>
#include "lexer.hpp"

void test_lexer0()
{
    riddle::lexer lex("bool b0; int factotum;");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::BOOL);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 3);
    auto id_tok = lex.next_token();
    assert(id_tok->sym == riddle::ID);
    assert(static_cast<riddle::id_token *>(id_tok.get())->id == "b0");
    assert(id_tok->line == 0);
    assert(id_tok->start_pos == 5);
    assert(id_tok->end_pos == 6);
    auto semicolon_tok = lex.next_token();
    assert(semicolon_tok->sym == riddle::SEMICOLON);
    assert(semicolon_tok->line == 0);
    assert(semicolon_tok->start_pos == 7);
    assert(semicolon_tok->end_pos == 7);
    auto int_tok = lex.next_token();
    assert(int_tok->sym == riddle::INT);
    assert(int_tok->line == 0);
    assert(int_tok->start_pos == 9);
    assert(int_tok->end_pos == 11);
    id_tok = lex.next_token();
    assert(id_tok->sym == riddle::ID);
    assert(static_cast<riddle::id_token *>(id_tok.get())->id == "factotum");
    assert(id_tok->line == 0);
    assert(id_tok->start_pos == 13);
    assert(id_tok->end_pos == 20);
    semicolon_tok = lex.next_token();
    assert(semicolon_tok->sym == riddle::SEMICOLON);
    assert(semicolon_tok->line == 0);
    assert(semicolon_tok->start_pos == 21);
    assert(semicolon_tok->end_pos == 21);
    auto eof_tok = lex.next_token();
    assert(eof_tok->sym == riddle::EoF);
}

void test_lexer1()
{
    riddle::lexer lex("5, .5, 2.5");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 5);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 1);
    auto comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 2);
    assert(comma_tok->end_pos == 2);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(static_cast<riddle::real_token *>(tok.get())->value == utils::rational(1, 2));
    assert(tok->line == 0);
    assert(tok->start_pos == 3);
    assert(tok->end_pos == 5);
    comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 6);
    assert(comma_tok->end_pos == 6);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(static_cast<riddle::real_token *>(tok.get())->value == utils::rational(5, 2));
    assert(tok->line == 0);
    assert(tok->start_pos == 7);
    assert(tok->end_pos == 10);
    auto eof_tok = lex.next_token();
    assert(eof_tok->sym == riddle::EoF);
}

void test_lexer2()
{
    riddle::lexer lex("[0, 10], ~(1/3, 1/2)");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::LBRACKET);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    
}

int main(int argc, char const *argv[])
{
    test_lexer0();
    test_lexer1();
    return 0;
}
