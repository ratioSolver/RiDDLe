#pragma once

#include <cstddef>

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

  class bool_token final : public token
  {
  public:
    bool_token(const bool &value, const size_t &start_line, const size_t &start_pos, const size_t &end_line, const size_t &end_pos) : token(BOOL_ID, start_line, start_pos, end_line, end_pos), value(value) {}
    virtual ~bool_token() = default;

    const bool value;
  };

  class lexer
  {
  };
} // namespace riddle
