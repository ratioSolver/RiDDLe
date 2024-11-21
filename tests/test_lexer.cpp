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
    riddle::lexer lex("5, .5, 2.5, 13.275");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 5);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 0);
    auto comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 1);
    assert(comma_tok->end_pos == 1);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(static_cast<riddle::real_token *>(tok.get())->value == utils::rational(1, 2));
    assert(tok->line == 0);
    assert(tok->start_pos == 3);
    assert(tok->end_pos == 4);
    comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 5);
    assert(comma_tok->end_pos == 5);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(static_cast<riddle::real_token *>(tok.get())->value == utils::rational(5, 2));
    assert(tok->line == 0);
    assert(tok->start_pos == 7);
    assert(tok->end_pos == 9);
    comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 10);
    assert(comma_tok->end_pos == 10);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(static_cast<riddle::real_token *>(tok.get())->value == utils::rational(531, 40));
    assert(tok->line == 0);
    assert(tok->start_pos == 12);
    assert(tok->end_pos == 17);
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
    assert(tok->end_pos == 0);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 0);
    assert(tok->line == 0);
    assert(tok->start_pos == 1);
    assert(tok->end_pos == 1);
    auto comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 2);
    assert(comma_tok->end_pos == 2);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 10);
    assert(tok->line == 0);
    assert(tok->start_pos == 4);
    assert(tok->end_pos == 5);
    auto rbracket_tok = lex.next_token();
    assert(rbracket_tok->sym == riddle::RBRACKET);
    assert(rbracket_tok->line == 0);
    assert(rbracket_tok->start_pos == 6);
    assert(rbracket_tok->end_pos == 6);
    comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 7);
    assert(comma_tok->end_pos == 7);
    tok = lex.next_token();
    assert(tok->sym == riddle::TILDE);
    assert(tok->line == 0);
    assert(tok->start_pos == 9);
    assert(tok->end_pos == 9);
    tok = lex.next_token();
    assert(tok->sym == riddle::LPAREN);
    assert(tok->line == 0);
    assert(tok->start_pos == 10);
    assert(tok->end_pos == 10);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 1);
    assert(tok->line == 0);
    assert(tok->start_pos == 11);
    assert(tok->end_pos == 11);
    tok = lex.next_token();
    assert(tok->sym == riddle::SLASH);
    assert(tok->line == 0);
    assert(tok->start_pos == 12);
    assert(tok->end_pos == 12);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 3);
    assert(tok->line == 0);
    assert(tok->start_pos == 13);
    assert(tok->end_pos == 13);
    comma_tok = lex.next_token();
    assert(comma_tok->sym == riddle::COMMA);
    assert(comma_tok->line == 0);
    assert(comma_tok->start_pos == 14);
    assert(comma_tok->end_pos == 14);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 1);
    assert(tok->line == 0);
    assert(tok->start_pos == 16);
    assert(tok->end_pos == 16);
    tok = lex.next_token();
    assert(tok->sym == riddle::SLASH);
    assert(tok->line == 0);
    assert(tok->start_pos == 17);
    assert(tok->end_pos == 17);
    tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(static_cast<riddle::int_token *>(tok.get())->value == 2);
    assert(tok->line == 0);
    assert(tok->start_pos == 18);
    assert(tok->end_pos == 18);
    tok = lex.next_token();
    assert(tok->sym == riddle::RPAREN);
    assert(tok->line == 0);
    assert(tok->start_pos == 19);
    assert(tok->end_pos == 19);
    auto eof_tok = lex.next_token();
    assert(eof_tok->sym == riddle::EoF);
}

void test_lexer4()
{
    riddle::lexer lex("int x0;\nreal ret;");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::INT);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 2);
    tok = lex.next_token();
    assert(tok->sym == riddle::ID);
    assert(static_cast<riddle::id_token *>(tok.get())->id == "x0");
    assert(tok->line == 0);
    assert(tok->start_pos == 4);
    assert(tok->end_pos == 5);
    tok = lex.next_token();
    assert(tok->sym == riddle::SEMICOLON);
    assert(tok->line == 0);
    assert(tok->start_pos == 6);
    assert(tok->end_pos == 6);
    tok = lex.next_token();
    assert(tok->sym == riddle::REAL);
    assert(tok->line == 1);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 3);
    tok = lex.next_token();
    assert(tok->sym == riddle::ID);
    assert(static_cast<riddle::id_token *>(tok.get())->id == "ret");
    assert(tok->line == 1);
    assert(tok->start_pos == 5);
    assert(tok->end_pos == 7);
    tok = lex.next_token();
    assert(tok->sym == riddle::SEMICOLON);
    assert(tok->line == 1);
    assert(tok->start_pos == 8);
    assert(tok->end_pos == 8);
    auto eof_tok = lex.next_token();
    assert(eof_tok->sym == riddle::EoF);
}

int main(int argc, char const *argv[])
{
    test_lexer0();
    test_lexer1();
    test_lexer2();
    test_lexer4();
    return 0;
}
