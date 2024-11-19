#pragma once

#include "rational.hpp"
#include <memory>

namespace riddle
{
  enum symbol
  {
    BOOL,      // `bool`
    INT,       // `int`
    REAL,      // `real`
    TIME,      // `time`
    STRING,    // `string`
    SEMICOLON, // `;`
    ID,        // (`a`..`z`|`A`..`Z`|`_`) (`a`..`z`|`A`..`Z`|`0`..`9`|`_`)*
    EoF        // End of file
  };

  class token
  {
  public:
    token(symbol sym, size_t line, size_t start_pos, size_t end_pos) noexcept : sym(sym), line(line), start_pos(start_pos), end_pos(end_pos) {}
    virtual ~token() noexcept = default;

    const symbol sym;
    const size_t line, start_pos, end_pos;
  };

  class id_token final : public token
  {
  public:
    id_token(std::string &&id, size_t line, size_t start_pos, size_t end_pos) noexcept : token(ID, line, start_pos, end_pos), id(std::move(id)) {}

    const std::string id;
  };

  class lexer final
  {
  public:
    lexer(const std::string &source);
    lexer(std::string &&source);
    lexer(std::istream &is);

    std::unique_ptr<token> next_token();

  private:
    static bool is_alpha(char ch) noexcept { return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
    static bool is_digit(char ch) noexcept { return ch >= '0' && ch <= '9'; }
    static bool is_alnum(char ch) noexcept { return is_alpha(ch) || is_digit(ch); }

    bool match(char expected) noexcept;
    char next() noexcept;
    std::string finish_id(std::string &str) noexcept;

    std::unique_ptr<token> make_token(symbol sym) noexcept;
    std::unique_ptr<token> make_id(std::string &&id) noexcept;

  private:
    std::string sb;
    size_t pos = 0;
    char ch;
    size_t line = 0, start_pos = 0, end_pos = 0;
  };
} // namespace riddle
