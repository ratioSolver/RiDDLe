#pragma once

#include "compilation_unit.hpp"

namespace riddle
{
  class parser final
  {
  public:
    parser(std::string &&source) : lex(std::move(source)) {}
    parser(std::istream &is) : lex(is) {}

    [[nodiscard]] std::unique_ptr<compilation_unit> compilation_unit();
    [[nodiscard]] std::unique_ptr<enum_declaration> enum_declaration();
    [[nodiscard]] std::unique_ptr<class_declaration> class_declaration();
    [[nodiscard]] std::unique_ptr<field_declaration> field_declaration();
    [[nodiscard]] std::unique_ptr<method_declaration> method_declaration();
    [[nodiscard]] std::unique_ptr<constructor_declaration> constructor_declaration();
    [[nodiscard]] std::unique_ptr<predicate_declaration> predicate_declaration();
    [[nodiscard]] std::unique_ptr<statement> statement();

  private:
    [[nodiscard]] const token *next_token();
    [[nodiscard]] bool match(const symbol &sym);
    void backtrack(const size_t &p) noexcept;

  private:
    lexer lex;                                        // The lexer object
    const token *tk = nullptr;                        // The current token
    std::vector<std::unique_ptr<const token>> tokens; // The tokens
    std::size_t pos = 0;                              // The current position in the tokens
  };
} // namespace riddle
