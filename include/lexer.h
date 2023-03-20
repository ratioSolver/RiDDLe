#pragma once

#include "riddle_export.h"
#include "rational.h"
#include "memory.h"
#include <istream>
#include <cmath>

#define BOOL_KW "bool"
#define INT_KW "int"
#define REAL_KW "real"
#define TIME_POINT_KW "time"
#define STRING_KW "string"
#define THIS_KW "this"
#define RETURN_KW "return"
#define TAU_KW "tau"

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

    virtual std::string to_string() const
    {
      switch (sym)
      {
      case BOOL_ID:
        return BOOL_KW;
      case INT_ID:
        return INT_KW;
      case REAL_ID:
        return REAL_KW;
      case TIME_ID:
        return TIME_POINT_KW;
      case STRING_ID:
        return STRING_KW;
      case TYPEDEF_ID:
        return "typedef";
      case ENUM_ID:
        return "enum";
      case CLASS_ID:
        return "class";
      case GOAL_ID:
        return "goal";
      case FACT_ID:
        return "fact";
      case PREDICATE_ID:
        return "predicate";
      case NEW_ID:
        return "new";
      case OR_ID:
        return "or";
      case FOR_ID:
        return "for";
      case THIS_ID:
        return THIS_KW;
      case VOID_ID:
        return "void";
      case RETURN_ID:
        return RETURN_KW;
      case DOT_ID:
        return ".";
      case COMMA_ID:
        return ",";
      case COLON_ID:
        return ":";
      case SEMICOLON_ID:
        return ";";
      case LPAREN_ID:
        return "(";
      case RPAREN_ID:
        return ")";
      case LBRACKET_ID:
        return "[";
      case RBRACKET_ID:
        return "]";
      case LBRACE_ID:
        return "{";
      case RBRACE_ID:
        return "}";
      case PLUS_ID:
        return "+";
      case MINUS_ID:
        return "-";
      case STAR_ID:
        return "*";
      case SLASH_ID:
        return "/";
      case AMP_ID:
        return "&";
      case BAR_ID:
        return "|";
      case EQ_ID:
        return "=";
      case GT_ID:
        return ">";
      case LT_ID:
        return "<";
      case BANG_ID:
        return "!";
      case EQEQ_ID:
        return "==";
      case LTEQ_ID:
        return "<=";
      case GTEQ_ID:
        return ">=";
      case BANGEQ_ID:
        return "!=";
      case IMPLICATION_ID:
        return "->";
      case CARET_ID:
        return "^";
      case ID_ID:
        return "id";
      case BoolLiteral_ID:
        return "bool";
      case IntLiteral_ID:
        return "int";
      case RealLiteral_ID:
        return "real";
      case StringLiteral_ID:
        return "string";
      case EOF_ID:
        return "EOF";
      default:
        return "unknown";
      }
    }

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

    std::string to_string() const override { return id; }

  public:
    const std::string id;
  };

  class bool_token final : public token
  {
  public:
    bool_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const bool &val) : token(BoolLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

    std::string to_string() const override { return val ? "true" : "false"; }

  public:
    const bool val;
  };

  class int_token final : public token
  {
  public:
    int_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const utils::I &val) : token(IntLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

    std::string to_string() const override { return std::to_string(val); }

  public:
    const utils::I val;
  };

  class real_token final : public token
  {
  public:
    real_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const utils::rational &val) : token(RealLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

    std::string to_string() const override { return std::to_string(static_cast<double>(val.numerator()) / val.denominator()); }

  public:
    const utils::rational val;
  };

  class string_token final : public token
  {
  public:
    string_token(const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos, const std::string &str) : token(StringLiteral_ID, start_line, start_pos, end_line, end_pos), str(str) {}

    std::string to_string() const override { return '\"' + str + '\"'; }

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
    /**
     * @brief Check if the character is a valid identifier part.
     *
     * @param ch The character to check.
     * @return true If the character is a valid identifier part.
     * @return false If the character is not a valid identifier part.
     */
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