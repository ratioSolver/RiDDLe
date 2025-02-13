#pragma once

#include "compilation_unit.hpp"

namespace riddle
{
  class parser final
  {
  public:
    parser(std::istream &is);

    [[nodiscard]] utils::u_ptr<compilation_unit> parse_compilation_unit();
    [[nodiscard]] utils::u_ptr<enum_declaration> parse_enum_declaration();
    [[nodiscard]] utils::u_ptr<class_declaration> parse_class_declaration();
    [[nodiscard]] utils::u_ptr<field_declaration> parse_field_declaration();
    [[nodiscard]] utils::u_ptr<method_declaration> parse_method_declaration();
    [[nodiscard]] utils::u_ptr<constructor_declaration> parse_constructor_declaration();
    [[nodiscard]] utils::u_ptr<predicate_declaration> parse_predicate_declaration();
    [[nodiscard]] utils::u_ptr<statement> parse_statement();
    [[nodiscard]] utils::s_ptr<expression> parse_expression(std::size_t precedence = 0);

  private:
    [[nodiscard]] bool match(const symbol &sym);

    void error(std::string &&err);

  private:
    lexer lex;                                     // The lexer object
    std::vector<utils::u_ptr<const token>> tokens; // The tokens
    std::size_t pos = 0;                           // The current position in the tokens
  };
} // namespace riddle
