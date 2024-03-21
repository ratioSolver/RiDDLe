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

    void declare(std::shared_ptr<scope> &scp) const;
    void refine(std::shared_ptr<scope> &scp) const;
    void refine_predicates(std::shared_ptr<scope> &scp) const;
    void execute(std::shared_ptr<scope> &scp, std::shared_ptr<env> &ctx) const;

  private:
    std::vector<std::unique_ptr<type_declaration>> types;           // The type declarations.
    std::vector<std::unique_ptr<method_declaration>> methods;       // The method declarations.
    std::vector<std::unique_ptr<predicate_declaration>> predicates; // The predicate declarations.
    std::vector<std::unique_ptr<statement>> body;                   // The statements.
  };
} // namespace riddle
