#pragma once

#include "declaration.hpp"

namespace riddle
{
  class compilation_unit
  {
  public:
    compilation_unit(std::vector<std::unique_ptr<type_declaration>> &&types, std::vector<std::unique_ptr<method_declaration>> &&methods, std::vector<std::unique_ptr<predicate_declaration>> &&predicates, std::vector<std::unique_ptr<statement>> &&statements) : types(std::move(types)), methods(std::move(methods)), predicates(std::move(predicates)), body(std::move(statements)) {}

    void declare(scope &scp) const;
    void refine(scope &scp) const;
    void refine_predicates(scope &scp) const;
    void execute(const scope &scp, env &ctx) const;

  private:
    std::vector<std::unique_ptr<type_declaration>> types;           // The type declarations.
    std::vector<std::unique_ptr<method_declaration>> methods;       // The method declarations.
    std::vector<std::unique_ptr<predicate_declaration>> predicates; // The predicate declarations.
    std::vector<std::unique_ptr<statement>> body;                   // The statements.
  };
} // namespace riddle