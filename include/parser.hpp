#pragma once

#include "compilation_unit.hpp"

namespace riddle
{
  class parser final
  {
  public:
    parser(std::istream &is);

    [[nodiscard]] std::unique_ptr<compilation_unit> parse_compilation_unit();
    [[nodiscard]] std::unique_ptr<enum_declaration> parse_enum_declaration();
    [[nodiscard]] std::unique_ptr<class_declaration> parse_class_declaration();
    [[nodiscard]] std::unique_ptr<field_declaration> parse_field_declaration();
    [[nodiscard]] std::unique_ptr<method_declaration> parse_method_declaration();
    [[nodiscard]] std::unique_ptr<constructor_declaration> parse_constructor_declaration();
    [[nodiscard]] std::unique_ptr<predicate_declaration> parse_predicate_declaration();
    [[nodiscard]] std::unique_ptr<statement> parse_statement();
    [[nodiscard]] std::unique_ptr<expression> parse_expression(std::size_t precedence = 0);

  private:
    [[nodiscard]] bool match(const symbol &sym);

    void error(std::string &&err);

  private:
    lexer lex;                                        // The lexer object
    std::vector<std::unique_ptr<const token>> tokens; // The tokens
    std::size_t pos = 0;                              // The current position in the tokens
  };
} // namespace riddle
