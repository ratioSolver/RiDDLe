#pragma once

#include <vector>
#include "lexer.hpp"

namespace riddle
{
  class compilation_unit
  {
  public:
    compilation_unit() = default;
  };

  class parser
  {
  public:
    parser(const std::string &in) : lex(in) {}
    parser(std::istream &in) : lex(in) {}

    std::unique_ptr<compilation_unit> parse();

  private:
    token *next_token();
    bool match(const symbol &sym);
    void backtrack(const size_t &p) noexcept;

    void error(const std::string &err);

  private:
    lexer lex;                                  // The lexer.
    const token *tk = nullptr;                  // The current lookahead token..
    std::vector<std::unique_ptr<token>> tokens; // The tokens.
    std::size_t pos = 0;                        // The current position in the tokens.
  };
} // namespace riddle
