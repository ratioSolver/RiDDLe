#pragma once

#include "declaration.hpp"

namespace riddle
{
  class compilation_unit
  {
  public:
    compilation_unit(std::vector<std::unique_ptr<type_declaration>> &&types, std::vector<std::unique_ptr<method_declaration>> &&methods, std::vector<std::unique_ptr<predicate_declaration>> &&predicates, std::vector<std::unique_ptr<statement>> &&statements) : types(std::move(types)), methods(std::move(methods)), predicates(std::move(predicates)), body(std::move(statements)) {}

    std::string to_string() const
    {
      std::string str;
      for (const auto &type : types)
        str += type->to_string() + "\n";
      for (const auto &method : methods)
        str += method->to_string() + "\n";
      for (const auto &predicate : predicates)
        str += predicate->to_string() + "\n";
      for (const auto &stmt : body)
        str += stmt->to_string() + "\n";
      return str;
    }

  private:
    std::vector<std::unique_ptr<type_declaration>> types;           // The type declarations.
    std::vector<std::unique_ptr<method_declaration>> methods;       // The method declarations.
    std::vector<std::unique_ptr<predicate_declaration>> predicates; // The predicate declarations.
    std::vector<std::unique_ptr<statement>> body;                   // The statements.
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

    std::unique_ptr<typedef_declaration> parse_typedef_declaration();
    std::unique_ptr<enum_declaration> parse_enum_declaration();
    std::unique_ptr<class_declaration> parse_class_declaration();
    std::unique_ptr<field_declaration> parse_field_declaration();
    std::unique_ptr<method_declaration> parse_method_declaration();
    std::unique_ptr<constructor_declaration> parse_constructor_declaration();
    std::unique_ptr<predicate_declaration> parse_predicate_declaration();
    std::unique_ptr<statement> parse_statement();
    std::unique_ptr<expression> parse_expression(const size_t &pr = 0);

    void error(const std::string &err);

  private:
    lexer lex;                                  // The lexer.
    const token *tk = nullptr;                  // The current lookahead token..
    std::vector<std::unique_ptr<token>> tokens; // The tokens.
    std::size_t pos = 0;                        // The current position in the tokens.
  };
} // namespace riddle
