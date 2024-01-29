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
    typedef_declaration(const id_token &&name, const id_token &&primitive_type, std::unique_ptr<expression> &&expr) : name(std::move(name)), primitive_type(std::move(primitive_type)), expr(std::move(expr)) {}

    std::string to_string() const override { return "typedef " + primitive_type.to_string() + " " + name.to_string() + " = " + expr->to_string() + ";"; }

  private:
    id_token name;
    id_token primitive_type;
    std::unique_ptr<expression> expr;
  };

  class enum_declaration final : public type_declaration
  {
  public:
    enum_declaration(const id_token &&name, std::vector<string_token> &&values, std::vector<std::vector<id_token>> &&enum_refs) : name(std::move(name)), values(std::move(values)), enum_refs(std::move(enum_refs)) {}

    std::string to_string() const override
    {
      std::string res = "enum " + name.to_string() + " {";
      if (!values.empty())
      {
        res += '\"' + values[0].to_string() + '\"';
        for (size_t i = 1; i < values.size(); ++i)
          res += ", \"" + values[i].to_string() + '\"';
        if (!enum_refs.empty())
          res += ", ";
      }
      if (!enum_refs.empty())
      {
        res += enum_refs[0][0].to_string();
        for (size_t i = 1; i < enum_refs[0].size(); ++i)
          res += "." + enum_refs[0][i].to_string();
        for (size_t i = 1; i < enum_refs.size(); ++i)
        {
          res += ", " + enum_refs[i][0].to_string();
          for (size_t j = 1; j < enum_refs[i].size(); ++j)
            res += "." + enum_refs[i][j].to_string();
        }
      }
      return res + "}";
    }

  private:
    id_token name;
    std::vector<string_token> values;
    std::vector<std::vector<id_token>> enum_refs;
  };

  class init_element
  {
  public:
    init_element(const id_token &&name, std::vector<std::unique_ptr<expression>> &&args) : name(std::move(name)), args(std::move(args)) {}
    init_element(init_element &&other) noexcept : name(std::move(other.name)), args(std::move(other.args)) {}

    std::string to_string() const
    {
      std::string res = name.to_string() + '(';
      if (!args.empty())
      {
        res += args[0]->to_string();
        for (size_t i = 1; i < args.size(); ++i)
          res += ", " + args[i]->to_string();
      }
      return res + ')';
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
      std::string res;
      if (!type_parameters.empty())
      {
        res += type_parameters[0].to_string();
        for (size_t i = 1; i < type_parameters.size(); ++i)
          res += ", " + type_parameters[i].to_string();
        res += " ";
      }
      res += inits[0].to_string();
      for (size_t i = 1; i < inits.size(); ++i)
        res += ", " + inits[i].to_string();
      return res + ";";
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
    method_declaration(std::vector<id_token> &&rt, const id_token &&name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::unique_ptr<statement>> &&stmts) : return_type(std::move(rt)), name(std::move(name)), parameters(std::move(params)), body(std::move(stmts)) {}

    std::string to_string() const
    {
      std::string res = "method " + name.to_string() + '(';
      if (!parameters.empty())
      {
        res += parameters[0].first[0].to_string() + " " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
          res += ", " + parameters[i].first[0].to_string() + " " + parameters[i].second.to_string();
      }
      res += ") : ";
      if (!return_type.empty())
      {
        res += return_type[0].to_string();
        for (size_t i = 1; i < return_type.size(); ++i)
          res += ", " + return_type[i].to_string();
      }
      res += " {\n";
      for (const auto &stmt : body)
        res += stmt->to_string() + "\n";
      return res + "}";
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
    predicate_declaration(const id_token &&name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::unique_ptr<statement>> &&stmts) : name(std::move(name)), parameters(std::move(params)), body(std::move(stmts)) {}

    std::string to_string() const
    {
      std::string res = "predicate " + name.to_string() + '(';
      if (!parameters.empty())
      {
        res += parameters[0].first[0].to_string() + " " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
          res += ", " + parameters[i].first[0].to_string() + " " + parameters[i].second.to_string();
      }
      res += ")";
      if (!base_predicates.empty())
      {
        res += " : " + base_predicates[0][0].to_string();
        for (size_t i = 1; i < base_predicates[0].size(); ++i)
          res += "." + base_predicates[0][i].to_string();
        for (size_t i = 1; i < base_predicates.size(); ++i)
        {
          res += ", " + base_predicates[i][0].to_string();
          for (size_t j = 1; j < base_predicates[i].size(); ++j)
            res += "." + base_predicates[i][j].to_string();
        }
      }
      res += " {\n";
      for (const auto &stmt : body)
        res += stmt->to_string() + "\n";
      return res + "}";
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
    class_declaration(const id_token &&name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::vector<id_token>> &&base_classes, std::vector<std::unique_ptr<field_declaration>> &&fields, std::vector<std::unique_ptr<constructor_declaration>> &&constructors, std::vector<std::unique_ptr<method_declaration>> &&methods, std::vector<std::unique_ptr<predicate_declaration>> &&predicates, std::vector<std::unique_ptr<type_declaration>> &&types) : name(std::move(name)), parameters(std::move(params)), base_classes(std::move(base_classes)), fields(std::move(fields)), constructors(std::move(constructors)), methods(std::move(methods)), predicates(std::move(predicates)), types(std::move(types)) {}

    std::string to_string() const override
    {
      std::string res = "class " + name.to_string();
      if (!parameters.empty())
      {
        res += '<' + parameters[0].first[0].to_string();
        for (size_t i = 1; i < parameters[0].first.size(); ++i)
          res += ", " + parameters[0].first[i].to_string();
        res += "> " + parameters[0].second.to_string();
        for (size_t i = 1; i < parameters.size(); ++i)
        {
          res += ", <" + parameters[i].first[0].to_string();
          for (size_t j = 1; j < parameters[i].first.size(); ++j)
            res += ", " + parameters[i].first[j].to_string();
          res += "> " + parameters[i].second.to_string();
        }
      }
      if (!base_classes.empty())
      {
        res += " : " + base_classes[0][0].to_string();
        for (size_t i = 1; i < base_classes[0].size(); ++i)
          res += "." + base_classes[0][i].to_string();
        for (size_t i = 1; i < base_classes.size(); ++i)
        {
          res += ", " + base_classes[i][0].to_string();
          for (size_t j = 1; j < base_classes[i].size(); ++j)
            res += "." + base_classes[i][j].to_string();
        }
      }
      res += " {\n";
      for (const auto &field : fields)
        res += field->to_string() + "\n";
      for (const auto &constructor : constructors)
        res += constructor->to_string() + "\n";
      for (const auto &method : methods)
        res += method->to_string() + "\n";
      for (const auto &predicate : predicates)
        res += predicate->to_string() + "\n";
      for (const auto &type : types)
        res += type->to_string() + "\n";
      return res + "}";
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
