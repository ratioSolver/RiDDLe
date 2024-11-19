#include <fstream>
#include <cassert>
#include "lexer.hpp"

void test_lexer()
{
    riddle::lexer lex("bool b0;");
    auto tok = lex.next_token();
    assert(tok->sym == riddle::BOOL_ID);
    assert(tok->line == 0);
    assert(tok->start_pos == 0);
    assert(tok->end_pos == 4);
    auto id_tok = lex.next_token();
    assert(id_tok->sym == riddle::ID_ID);
    assert(static_cast<riddle::id_token *>(id_tok.get())->id == "b0");
    assert(id_tok->line == 0);
    assert(id_tok->start_pos == 5);
    assert(id_tok->end_pos == 7);
    auto semicolon_tok = lex.next_token();
    assert(semicolon_tok->sym == riddle::SEMICOLON_ID);
    assert(semicolon_tok->line == 0);
    assert(semicolon_tok->start_pos == 8);
    assert(semicolon_tok->end_pos == 8);
    auto eof_tok = lex.next_token();
    assert(eof_tok == nullptr);
}

int main(int argc, char const *argv[])
{
    test_lexer();
    return 0;
}
