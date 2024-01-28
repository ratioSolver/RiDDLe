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
    typedef_declaration(const id_token &name, const id_token &primitive_type, std::unique_ptr<expression> &&expr) : name(name), primitive_type(primitive_type), expr(std::move(expr)) {}

    std::string to_string() const override { return "typedef " + primitive_type.to_string() + " " + name.to_string() + " = " + expr->to_string() + ";"; }

  private:
    id_token name;
    id_token primitive_type;
    std::unique_ptr<expression> expr;
  };

  class enum_declaration final : public type_declaration
  {
  public:
    enum_declaration(const id_token &name, std::vector<string_token> &&values, std::vector<std::vector<id_token>> &&enum_refs) : name(name), values(std::move(values)), enum_refs(std::move(enum_refs)) {}

    std::string to_string() const override
    {
      std::string result = "enum " + name.to_string() + " {";
      if (!values.empty())
      {
        result += '\"' + values[0].to_string() + '\"';
        for (size_t i = 1; i < values.size(); ++i)
          result += ", \"" + values[i].to_string() + '\"';
        if (!enum_refs.empty())
          result += ", ";
      }
      if (!enum_refs.empty())
      {
        result += enum_refs[0][0].to_string();
        for (size_t i = 1; i < enum_refs[0].size(); ++i)
          result += "." + enum_refs[0][i].to_string();
        for (size_t i = 1; i < enum_refs.size(); ++i)
        {
          result += ", " + enum_refs[i][0].to_string();
          for (size_t j = 1; j < enum_refs[i].size(); ++j)
            result += "." + enum_refs[i][j].to_string();
        }
      }
      return result + "}";
    }

  private:
    id_token name;
    std::vector<string_token> values;
    std::vector<std::vector<id_token>> enum_refs;
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
