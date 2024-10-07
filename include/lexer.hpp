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

    virtual std::string to_string() const
    {
      switch (sym)
      {
      case BOOL_ID:
        return "bool";
      case INT_ID:
        return "int";
      case REAL_ID:
        return "real";
      case TIME_ID:
        return "time";
      case STRING_ID:
        return "string";
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
        return "this";
      case VOID_ID:
        return "void";
      case RETURN_ID:
        return "return";
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

    const symbol sym;
    const size_t start_line, start_pos, end_line, end_pos;
  };

  class id_token final : public token
  {
  public:
    id_token(std::string_view id, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(ID_ID, start_line, start_pos, end_line, end_pos), id(id) {}

    std::string to_string() const override { return id; }

    const std::string id;
  };

  class bool_token final : public token
  {
  public:
    bool_token(const bool &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(BoolLiteral_ID, start_line, start_pos, end_line, end_pos), value(value) {}

    std::string to_string() const override { return value ? "true" : "false"; }

    const bool value;
  };

  class int_token final : public token
  {
  public:
    int_token(const INT_TYPE &val, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(IntLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

    std::string to_string() const override { return std::to_string(val); }

    const INT_TYPE val;
  };

  class real_token final : public token
  {
  public:
    real_token(const utils::rational &val, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(RealLiteral_ID, start_line, start_pos, end_line, end_pos), val(val) {}

    std::string to_string() const override { return std::to_string(static_cast<double>(val.numerator()) / val.denominator()); }

    const utils::rational val;
  };

  class string_token final : public token
  {
  public:
    string_token(const std::string &str, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(StringLiteral_ID, start_line, start_pos, end_line, end_pos), str(str) {}

    std::string to_string() const override { return '\"' + str + '\"'; }

    const std::string str;
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
      auto tkn = std::make_unique<int_token>(static_cast<INT_TYPE>(std::stol(str)), start_line, start_pos, end_line, end_pos);
      start_line = end_line;
      start_pos = end_pos;
      return tkn;
    }

    std::unique_ptr<token> make_real_token(const std::string &intgr, const std::string &dec)
    {
      auto tkn = std::make_unique<real_token>(utils::rational(static_cast<INT_TYPE>(std::stol(intgr + dec)), static_cast<INT_TYPE>(std::pow(10, dec.size()))), start_line, start_pos, end_line, end_pos);
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
