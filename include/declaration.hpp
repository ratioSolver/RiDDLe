#pragma once

#include <vector>
#include "statement.hpp"

namespace riddle
{
  class type_declaration
  {
  public:
    type_declaration() = default;
    virtual ~type_declaration() = default;

    virtual std::string to_string() const = 0;
  };

  class typedef_declaration final : public type_declaration
  {
  public:
    std::string to_string() const override { return ""; }
  };

  class enum_declaration final : public type_declaration
  {
  public:
    std::string to_string() const override { return ""; }
  };

  class class_declaration final : public type_declaration
  {
  public:
    std::string to_string() const override { return ""; }
  };

  class field_declaration
  {
  public:
    std::string to_string() const { return ""; }
  };

  class method_declaration
  {
  public:
    std::string to_string() const { return ""; }
  };

  class constructor_declaration
  {
  public:
    std::string to_string() const { return ""; }
  };

  class predicate_declaration
  {
  public:
    std::string to_string() const { return ""; }
  };
} // namespace riddle
