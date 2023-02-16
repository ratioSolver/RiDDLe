#pragma once

#include "riddle_export.h"
#include "rational.h"
#include "memory.h"
#include <istream>
#include <cmath>

#define BOOL_KW "bool"
#define INT_KW "int"
#define REAL_KW "real"
#define TIME_KW "time"
#define STRING_KW "string"

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

  public:
    const symbol sym;
    const size_t start_line;
    const size_t start_pos;
    const size_t end_line;
    const size_t end_pos;
  };
  using token_ptr = utils::u_ptr<const token>;

  class id_token final : public token
  {
  public:
    id_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const std::string &id) : token(ID_ID, start_line, start_pos, end_line, end_pos), id(id) {}

  public:
    const std::string id;
  };

  class bool_token final : public token
  {
  public:
    bool_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const bool &val) : token(BoolLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

  public:
    const bool val;
  };

  class int_token final : public token
  {
  public:
    int_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const utils::I &val) : token(IntLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

  public:
    const utils::I val;
  };

  class real_token final : public token
  {
  public:
    real_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const utils::rational &val) : token(RealLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

  public:
    const utils::rational val;
  };

  class string_token final : public token
  {
  public:
    string_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const std::string &str) : token(StringLiteral_ID, start_line, start_pos, end_line, end_pos), str(str) {}

  public:
    const std::string str;
  };

  class lexer final
  {
  public:
    RIDDLE_EXPORT lexer(const std::string &str);
    RIDDLE_EXPORT lexer(std::istream &is);
    lexer(const lexer &orig) = delete;

    RIDDLE_EXPORT token_ptr next();

  private:
    static bool is_id_part(const char &ch) noexcept { return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'); }

    token_ptr mk_token(const symbol &sym) noexcept
    {
      auto tk = new const token(sym, start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr mk_id_token(const std::string &id) noexcept
    {
      auto tk = new const id_token(start_line, start_pos, end_line, end_pos, id);
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr mk_bool_token(const bool &val) noexcept
    {
      auto tk = new const bool_token(start_line, start_pos, end_line, end_pos, val);
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr mk_integer_token(const std::string &str) noexcept
    {
      auto tk = new const int_token(start_line, start_pos, end_line, end_pos, static_cast<utils::I>(std::stol(str)));
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr mk_rational_token(const std::string &intgr, const std::string &dec) noexcept
    {
      auto tk = new const real_token(start_line, start_pos, end_line, end_pos, utils::rational(static_cast<utils::I>(std::stol(intgr + dec)), static_cast<utils::I>(std::pow(10, dec.size()))));
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr mk_string_token(const std::string &str) noexcept
    {
      auto tk = new const string_token(start_line, start_pos, end_line, end_pos, str);
      start_line = end_line;
      start_pos = end_pos;
      return tk;
    }

    token_ptr finish_id(std::string &str) noexcept;

    void error(const std::string &err);
    char next_char() noexcept;

  private:
    std::string sb;
    size_t pos = 0;
    char ch;
    size_t start_line = 0;
    size_t start_pos = 0;
    size_t end_line = 0;
    size_t end_pos = 0;
  };
} // namespace riddle