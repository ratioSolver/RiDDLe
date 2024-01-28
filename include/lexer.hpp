#pragma once

#include <cstddef>
#include <istream>
#include <memory>
#include <cmath>
#include "rational.hpp"

namespace riddle
{
  enum symbol
  {
    BOOL_ID,          // `bool`
    INT_ID,           // `int`
    REAL_ID,          // `real`
    TIME_ID,          // `time`
    STRING_ID,        // `string`
    TYPEDEF_ID,       // `typedef`
    ENUM_ID,          // `enum`
    CLASS_ID,         // `class`
    GOAL_ID,          // `goal`
    FACT_ID,          // `fact`
    PREDICATE_ID,     // `predicate`
    NEW_ID,           // `new`
    OR_ID,            // `or`
    FOR_ID,           // `for`
    THIS_ID,          // `this`
    VOID_ID,          // `void`
    RETURN_ID,        // `return`
    DOT_ID,           // `.`
    COMMA_ID,         // `,`
    COLON_ID,         // `:`
    SEMICOLON_ID,     // `;`
    LPAREN_ID,        // `(`
    RPAREN_ID,        // `)`
    LBRACKET_ID,      // `[`
    RBRACKET_ID,      // `]`
    LBRACE_ID,        // `{`
    RBRACE_ID,        // `}`
    PLUS_ID,          // `+`
    MINUS_ID,         // `-`
    STAR_ID,          // `*`
    SLASH_ID,         // `/`
    AMP_ID,           // `&`
    BAR_ID,           // `|`
    EQ_ID,            // `=`
    GT_ID,            // `>`
    LT_ID,            // `<`
    BANG_ID,          // `!`
    EQEQ_ID,          // `==`
    LTEQ_ID,          // `<=`
    GTEQ_ID,          // `>=`
    BANGEQ_ID,        // `!=`
    IMPLICATION_ID,   // `->`
    CARET_ID,         // `^`
    ID_ID,            // (`a`..`z`|`A`..`Z`|`_`) (`a`..`z`|`A`..`Z`|`0`..`9`|`_`)*
    BoolLiteral_ID,   // `true` | `false`
    IntLiteral_ID,    // [0-9]+
    RealLiteral_ID,   // [0-9]+ `.` [0-9]+)? | `.` [0-9]+
    StringLiteral_ID, // `" . . ."`
    EOF_ID
  };

  class token
  {
  public:
    token(const symbol &sym, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : sym(sym), start_line(start_line), start_pos(start_pos), end_line(end_line), end_pos(end_pos) {}
    virtual ~token() = default;

    const symbol sym;
    const size_t start_line, start_pos, end_line, end_pos;
  };

  class id_token final : public token
  {
  public:
    id_token(const std::string &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(ID_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    const std::string value;
  };

  class bool_token final : public token
  {
  public:
    bool_token(const bool &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(BOOL_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    const bool value;
  };

  class int_token final : public token
  {
  public:
    int_token(const INTEGER_TYPE &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(INT_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    const INTEGER_TYPE value;
  };

  class real_token final : public token
  {
  public:
    real_token(const utils::rational &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(REAL_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    const utils::rational value;
  };

  class string_token final : public token
  {
  public:
    string_token(const std::string &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(STRING_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    const std::string value;
  };

  class lexer final
  {
  public:
    lexer(const std::string &source);
    lexer(std::istream &is);

    std::unique_ptr<token> next_token();

  private:
    /**
     * @brief Check if the character is a valid identifier part.
     *
     * @param ch The character to check.
     * @return true If the character is a valid identifier part.
     * @return false If the character is not a valid identifier part.
     */
    static bool is_id_part(const char &ch) noexcept { return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'); }

    char next_char();

    std::unique_ptr<token> finish_id(std::string &str) noexcept;

    std::unique_ptr<token> make_token(const symbol &sym)
    {
      auto tkn = std::make_unique<token>(sym, start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_id_token(const std::string &id)
    {
      auto tkn = std::make_unique<id_token>(id, start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_bool_token(const bool &b)
    {
      auto tkn = std::make_unique<bool_token>(b, start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_int_token(const std::string &str)
    {
      auto tkn = std::make_unique<int_token>(static_cast<INTEGER_TYPE>(std::stol(str)), start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_real_token(const std::string &intgr, const std::string &dec)
    {
      auto tkn = std::make_unique<real_token>(utils::rational(static_cast<INTEGER_TYPE>(std::stol(intgr + dec)), static_cast<INTEGER_TYPE>(std::pow(10, dec.size()))), start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_string_token(const std::string &str)
    {
      auto tkn = std::make_unique<string_token>(str, start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    void error(const std::string &err);

  private:
    std::string sb;
    size_t pos = 0;
    char ch;
    size_t start_line = 0, start_pos = 0, end_line = 0, end_pos = 0;
  };
} // namespace riddle
