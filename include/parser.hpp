#pragma once

#include "compilation_unit.hpp"

namespace riddle
{
  class parser
  {
  public:
    parser(const std::string &in) : lex(in) {}
    parser(std::istream &in) : lex(in) {}

    [[nodiscard]] std::unique_ptr<compilation_unit> parse();

  private:
    [[nodiscard]] const token *next_token();
    [[nodiscard]] bool match(const symbol &sym);
    void backtrack(const size_t &p) noexcept;

    [[nodiscard]] std::unique_ptr<typedef_declaration> parse_typedef_declaration();
    [[nodiscard]] std::unique_ptr<enum_declaration> parse_enum_declaration();
    [[nodiscard]] std::unique_ptr<class_declaration> parse_class_declaration();
    [[nodiscard]] std::unique_ptr<field_declaration> parse_field_declaration();
    [[nodiscard]] std::unique_ptr<method_declaration> parse_method_declaration();
    [[nodiscard]] std::unique_ptr<constructor_declaration> parse_constructor_declaration();
    [[nodiscard]] std::unique_ptr<predicate_declaration> parse_predicate_declaration();
    [[nodiscard]] std::unique_ptr<statement> parse_statement();
    [[nodiscard]] std::unique_ptr<expression> parse_expression(const size_t &pr = 0);

    void error(const std::string &err);

  private:
    lexer lex;                                        // The lexer.
    const token *tk = nullptr;                        // The current lookahead token..
    std::vector<std::unique_ptr<const token>> tokens; // The tokens.
    std::size_t pos = 0;                              // The current position in the tokens.
  };
} // namespace riddle
