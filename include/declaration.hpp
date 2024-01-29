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

  class init_element
  {
  public:
    init_element(const id_token &name, std::vector<std::unique_ptr<expression>> &&args) : name(name), args(std::move(args)) {}
    init_element(init_element &&other) noexcept : name(std::move(other.name)), args(std::move(other.args)) {}

    std::string to_string() const
    {
      std::string result = name.to_string() + '(';
      if (!args.empty())
      {
        result += args[0]->to_string();
        for (size_t i = 1; i < args.size(); ++i)
          result += ", " + args[i]->to_string();
      }
      return result + ')';
    }

  private:
    id_token name;
    std::vector<std::unique_ptr<expression>> args;
  };

  class field_declaration
  {
  public:
    field_declaration(std::vector<id_token> &&tp, std::vector<init_element> &&inits) : type_parameters(std::move(tp)), inits(std::move(inits)) {}

    std::string to_string() const
    {
      std::string result;
      if (!type_parameters.empty())
      {
        result += type_parameters[0].to_string();
        for (size_t i = 1; i < type_parameters.size(); ++i)
          result += ", " + type_parameters[i].to_string();
        result += " ";
      }
      result += inits[0].to_string();
      for (size_t i = 1; i < inits.size(); ++i)
        result += ", " + inits[i].to_string();
      return result + ";";
    }

  private:
    std::vector<id_token> type_parameters;
    std::vector<init_element> inits;
  };

  class constructor_declaration
  {
  public:
    constructor_declaration(std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<init_element> &&inits, std::vector<std::unique_ptr<statement>> &&stmts) : parameters(std::move(params)), inits(std::move(inits)), body(std::move(stmts)) {}

    std::string to_string() const { return ""; }

  private:
    std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the parameters of the constructor..
    std::vector<init_element> inits;                                    // the initializations of the fields..
    std::vector<std::unique_ptr<statement>> body;                       // the body of the constructor..
  };

  class method_declaration
  {
  public:
    method_declaration(std::vector<id_token> &&rt, const id_token &name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::unique_ptr<statement>> &&stmts) : return_type(std::move(rt)), name(name), parameters(std::move(params)), body(std::move(stmts)) {}

    std::string to_string() const
    {
      std::string result = "method " + name.to_string() + '(';
      if (!parameters.empty())
      {
        result += parameters[0].first[0].to_string() + " " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
          result += ", " + parameters[i].first[0].to_string() + " " + parameters[i].second.to_string();
      }
      result += ") : ";
      if (!return_type.empty())
      {
        result += return_type[0].to_string();
        for (size_t i = 1; i < return_type.size(); ++i)
          result += ", " + return_type[i].to_string();
      }
      result += " {\n";
      for (const auto &stmt : body)
        result += stmt->to_string() + "\n";
      return result + "}";
    }

  private:
    std::vector<id_token> return_type;                                  // the return type of the method..
    id_token name;                                                      // the name of the method..
    std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the parameters of the method..
    std::vector<std::unique_ptr<statement>> body;                       // the body of the method..
  };

  class predicate_declaration
  {
  public:
    predicate_declaration(const id_token &name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::unique_ptr<statement>> &&stmts) : name(name), parameters(std::move(params)), body(std::move(stmts)) {}

    std::string to_string() const
    {
      std::string result = "predicate " + name.to_string() + '(';
      if (!parameters.empty())
      {
        result += parameters[0].first[0].to_string() + " " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
          result += ", " + parameters[i].first[0].to_string() + " " + parameters[i].second.to_string();
      }
      result += ")";
      if (!base_predicates.empty())
      {
        result += " : " + base_predicates[0][0].to_string();
        for (size_t i = 1; i < base_predicates[0].size(); ++i)
          result += "." + base_predicates[0][i].to_string();
        for (size_t i = 1; i < base_predicates.size(); ++i)
        {
          result += ", " + base_predicates[i][0].to_string();
          for (size_t j = 1; j < base_predicates[i].size(); ++j)
            result += "." + base_predicates[i][j].to_string();
        }
      }
      result += " {\n";
      for (const auto &stmt : body)
        result += stmt->to_string() + "\n";
      return result + "}";
    }

  private:
    id_token name;                                                      // the name of the class..
    std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the type parameters of the class..
    std::vector<std::vector<id_token>> base_predicates;                 // the base predicates of the class..
    std::vector<std::unique_ptr<statement>> body;                       // the body of the rule..
  };

  class class_declaration final : public type_declaration
  {
  public:
    class_declaration(const id_token &name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::vector<id_token>> &&base_classes, std::vector<std::unique_ptr<field_declaration>> &&fields, std::vector<std::unique_ptr<constructor_declaration>> &&constructors, std::vector<std::unique_ptr<method_declaration>> &&methods, std::vector<std::unique_ptr<predicate_declaration>> &&predicates, std::vector<std::unique_ptr<type_declaration>> &&types) : name(name), parameters(std::move(params)), base_classes(std::move(base_classes)), fields(std::move(fields)), constructors(std::move(constructors)), methods(std::move(methods)), predicates(std::move(predicates)), types(std::move(types)) {}

    std::string to_string() const override
    {
      std::string result = "class " + name.to_string();
      if (!parameters.empty())
      {
        result += '<' + parameters[0].first[0].to_string();
        for (size_t i = 1; i < parameters[0].first.size(); ++i)
          result += ", " + parameters[0].first[i].to_string();
        result += "> " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
        {
          result += ", <" + parameters[i].first[0].to_string();
          for (size_t j = 1; j < parameters[i].first.size(); ++j)
            result += ", " + parameters[i].first[j].to_string();
          result += "> " + parameters[i].second.to_string();
        }
      }
      if (!base_classes.empty())
      {
        result += " : " + base_classes[0][0].to_string();
        for (size_t i = 1; i < base_classes[0].size(); ++i)
          result += "." + base_classes[0][i].to_string();
        for (size_t i = 1; i < base_classes.size(); ++i)
        {
          result += ", " + base_classes[i][0].to_string();
          for (size_t j = 1; j < base_classes[i].size(); ++j)
            result += "." + base_classes[i][j].to_string();
        }
      }
      result += " {\n";
      for (const auto &field : fields)
        result += field->to_string() + "\n";
      for (const auto &constructor : constructors)
        result += constructor->to_string() + "\n";
      for (const auto &method : methods)
        result += method->to_string() + "\n";
      for (const auto &predicate : predicates)
        result += predicate->to_string() + "\n";
      for (const auto &type : types)
        result += type->to_string() + "\n";
      return result + "}";
    }

  private:
    id_token name;                                                      // the name of the class..
    std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the type parameters of the class..
    std::vector<std::vector<id_token>> base_classes;                    // the base classes of the class..
    std::vector<std::unique_ptr<field_declaration>> fields;             // the fields of the class..
    std::vector<std::unique_ptr<constructor_declaration>> constructors; // the constructors of the class..
    std::vector<std::unique_ptr<method_declaration>> methods;           // the methods of the class..
    std::vector<std::unique_ptr<predicate_declaration>> predicates;     // the predicates of the class..
    std::vector<std::unique_ptr<type_declaration>> types;               // the types of the class..
  };
} // namespace riddle
