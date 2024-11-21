#pragma once

#include "rational.hpp"
#include <memory>

namespace riddle
{
  enum symbol
  {
    BOOL,        // `bool`
    INT,         // `int`
    REAL,        // `real`
    TIME,        // `time`
    STRING,      // `string`
    ID,          // (`a`..`z`|`A`..`Z`|`_`)(`a`..`z`|`A`..`Z`|`0`..`9`|`_`)*
    Bool,        // `true`|`false`
    Int,         // (`0`..`9`)+
    Real,        // ((`0`..`9`)+`.`(`0`..`9`)+)|(`.`(`0`..`9`)+)
    String,      // `"`(`\`|`"`|`[^"]`)*`"`
    ENUM,        // `enum`
    CLASS,       // `class`
    PREDICATE,   // `predicate`
    NEW,         // `new`
    FOR,         // `for`
    THIS,        // `this`
    AUTO,        // `auto`
    VOID,        // `void`
    RETURN,      // `return`
    FACT,        // `fact`
    GOAL,        // `goal`
    OR,          // `or`
    DOT,         // `.`
    COMMA,       // `,`
    COLON,       // `:`
    SEMICOLON,   // `;`
    LPAREN,      // `(`
    RPAREN,      // `)`
    LBRACKET,    // `[`
    RBRACKET,    // `]`
    LBRACE,      // `{`
    RBRACE,      // `}`
    PLUS,        // `+`
    MINUS,       // `-`
    STAR,        // `*`
    SLASH,       // `/`
    AMP,         // `&`
    BAR,         // `|`
    TILDE,       // `~`
    EQ,          // `=`
    GT,          // `>`
    LT,          // `<`
    BANG,        // `!`
    EQEQ,        // `==`
    LTEQ,        // `<=`
    GTEQ,        // `>=`
    BANGEQ,      // `!=`
    IMPLICATION, // `->`
    CARET,       // `^`
    EoF          // End of file
  };

  /**
   * @class token lexer.hpp "include/lexer.hpp"
   * @brief Represents a lexical token with its associated symbol and position information.
   *
   * The token class encapsulates a symbol and its position within the source code,
   * including the line number and start/end positions.
   *
   * @param sym The symbol associated with the token.
   * @param line The line number where the token is located.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class token
  {
  public:
    token(symbol sym, size_t line, size_t start_pos, size_t end_pos) noexcept : sym(sym), line(line), start_pos(start_pos), end_pos(end_pos) {}
    virtual ~token() noexcept = default;

    const symbol sym;                      // The symbol associated with the token
    const size_t line, start_pos, end_pos; // The line number and start/end positions of the token
  };

  /**
   * @class id_token lexer.hpp "include/lexer.hpp"
   * @brief Represents an identifier token in the lexer.
   *
   * This class is a final derived class of the token class and is used to represent
   * identifier tokens in the lexical analysis phase.
   *
   * @note This class is marked as final and cannot be inherited from.
   *
   * @param id The identifier string.
   * @param line The line number where the token is found.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class id_token final : public token
  {
  public:
    id_token(std::string &&id, size_t line, size_t start_pos, size_t end_pos) noexcept : token(ID, line, start_pos, end_pos), id(std::move(id)) {}

    const std::string id; // The identifier string
  };

  /**
   * @class bool_token lexer.hpp "include/lexer.hpp"
   * @brief Represents a boolean token in the lexer.
   *
   * This class is a final derived class of the token class, specifically for boolean values.
   *
   * @param value The boolean value of the token.
   * @param line The line number where the token is found.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class bool_token final : public token
  {
  public:
    bool_token(bool value, size_t line, size_t start_pos, size_t end_pos) noexcept : token(BOOL, line, start_pos, end_pos), value(value) {}

    const bool value; // The boolean value
  };

  /**
   * @class int_token lexer.hpp "include/lexer.hpp"
   * @brief Represents an integer token in the lexer.
   *
   * This class is a final derived class from the base class token, specifically
   * for handling integer tokens.
   *
   * @note This class is marked as final and cannot be inherited from.
   *
   * @param value The integer value of the token.
   * @param line The line number where the token is found.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class int_token final : public token
  {
  public:
    int_token(INT_TYPE value, size_t line, size_t start_pos, size_t end_pos) noexcept : token(INT, line, start_pos, end_pos), value(value) {}

    const INT_TYPE value; // The integer value
  };

  /**
   * @class real_token lexer.hpp "include/lexer.hpp"
   * @brief Represents a token for real numbers in the lexer.
   *
   * This class is a final derived class from the base class token. It is used to
   * represent real number tokens in the lexer with additional information such as
   * the value of the real number, the line number, and the start and end positions
   * in the source code.
   *
   * @param value The rational value of the real number token.
   * @param line The line number where the token is found.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class real_token final : public token
  {
  public:
    real_token(utils::rational &&value, size_t line, size_t start_pos, size_t end_pos) noexcept : token(REAL, line, start_pos, end_pos), value(std::move(value)) {}

    const utils::rational value; // The rational value of the real number
  };

  /**
   * @class string_token lexer.hpp "include/lexer.hpp"
   * @brief Represents a token for string literals in the lexer.
   *
   * This class is a final derived class from the base class token. It is used to
   * represent string tokens in the lexical analysis phase.
   *
   * @note This class is marked as final and cannot be inherited from.
   *
   * @param value The string value of the token.
   * @param line The line number where the token is found.
   * @param start_pos The starting position of the token in the line.
   * @param end_pos The ending position of the token in the line.
   */
  class string_token final : public token
  {
  public:
    string_token(std::string &&value, size_t line, size_t start_pos, size_t end_pos) noexcept : token(STRING, line, start_pos, end_pos), value(std::move(value)) {}

    const std::string value; // The string value
  };

  /**
   * @class lexer lexer.hpp "include/lexer.hpp"
   * @brief A lexical analyzer for the RiDDLe programming language.
   *
   * The lexer class is responsible for tokenizing the input source code and
   * generating a sequence of tokens that can be used by the parser to build
   * an abstract syntax tree (AST).
   */
  class lexer final
  {
  public:
    lexer(std::string &&source);
    lexer(std::istream &is);

    /**
     * @brief Retrieves the next token from the input stream.
     *
     * This function processes the input stream and returns the next token
     * encapsulated in a std::unique_ptr. If there are no more tokens to be
     * processed, it returns a nullptr.
     *
     * @return std::unique_ptr<token> A unique pointer to the next token, or nullptr if no more tokens are available.
     */
    std::unique_ptr<token> next_token();

  private:
    static bool is_alpha(char ch) noexcept { return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
    static bool is_digit(char ch) noexcept { return ch >= '0' && ch <= '9'; }
    static bool is_alnum(char ch) noexcept { return is_alpha(ch) || is_digit(ch); }

    bool match(char expected) noexcept;
    char next() noexcept;
    std::string finish_id(std::string &&str) noexcept;

    std::unique_ptr<token> make_token(symbol sym) noexcept;
    std::unique_ptr<id_token> make_id(std::string &&id) noexcept;
    std::unique_ptr<bool_token> make_bool(bool value) noexcept;
    std::unique_ptr<int_token> make_int(INT_TYPE value) noexcept;
    std::unique_ptr<real_token> make_real(utils::rational &&value) noexcept;
    std::unique_ptr<string_token> make_string(std::string &&value) noexcept;

  private:
    std::string sb;
    size_t pos = 0;
    char ch;
    size_t line = 0, start_pos = 0, end_pos = 0;
  };
} // namespace riddle
