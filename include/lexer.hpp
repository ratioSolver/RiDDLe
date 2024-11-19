#pragma once

#include "rational.hpp"

namespace riddle
{
  enum symbol
  {
  };

  class lexer final
  {
  public:
    lexer(const std::string &source);
    lexer(std::string &&source);
    lexer(std::istream &is);

  private:
    std::string sb;
    size_t pos = 0;
    char ch;
    size_t line = 0, start_pos = 0, end_pos = 0;
  };
} // namespace riddle
