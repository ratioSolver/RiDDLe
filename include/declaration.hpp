#pragma once

#include "statement.hpp"

namespace riddle
{
  class compilation_unit;
  class class_declaration;

  class type_declaration
  {
    friend class compilation_unit;
    friend class class_declaration;

  public:
    type_declaration() = default;
    virtual ~type_declaration() = default;

  private:
    /**
     * @brief Declares a type within the given scope.
     *
     * @param scope A reference to the scope in which the declaration is made.
     */
    virtual void declare(scope &) const {}
    /**
     * @brief Refines a type within the given scope.
     *
     * This method is called after all types have been declared, and is used to refine the types with additional information (e.g. fields, methods, predicates, etc.) that require the types to be fully declared.
     *
     * @param scope A reference to the scope in which the declaration is made.
     */
    virtual void refine(scope &) const {}
    /**
     * @brief Refines the predicates of a type within the given scope.
     *
     * This method is called after all types have been refined, and is used to refine the predicates with additional information (e.g. parent predicates) that require the predicates to be fully declared.
     *
     * @param scope A reference to the scope in which the declaration is made.
     */
    virtual void refine_predicates(scope &) const {}
  };

  class enum_declaration final : public type_declaration
  {
  public:
    enum_declaration(const id_token &&name, std::vector<string_token> &&values, std::vector<std::vector<id_token>> &&enum_refs) : name(std::move(name)), values(std::move(values)), enum_refs(std::move(enum_refs)) {}

  private:
    void declare(scope &scp) const override;
    void refine(scope &scp) const override;

  private:
    id_token name;
    std::vector<string_token> values;
    std::vector<std::vector<id_token>> enum_refs;
  };

  class field_declaration final
  {
    friend class class_declaration;

  public:
    field_declaration(std::vector<id_token> &&tp, std::vector<std::pair<id_token, std::unique_ptr<expression>>> &&fields) : tp(std::move(tp)), fields(std::move(fields)) {}

  private:
    void refine(scope &scp) const;

  private:
    std::vector<id_token> tp;
    std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;
  };

  class constructor_declaration final
  {
    friend class class_declaration;

  public:
    constructor_declaration(std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<id_token> &&names, std::vector<std::vector<std::unique_ptr<expression>>> &&args, std::vector<std::unique_ptr<statement>> &&stmts) : params(std::move(params)), names(std::move(names)), args(std::move(args)), stmts(std::move(stmts)) {}

  private:
    void refine(scope &scp) const;

  private:
    std::vector<std::pair<std::vector<id_token>, id_token>> params;
    std::vector<id_token> names;
    std::vector<std::vector<std::unique_ptr<expression>>> args;
    std::vector<std::unique_ptr<statement>> stmts;
  };

  class method_declaration final
  {
    friend class compilation_unit;
    friend class class_declaration;

  public:
    method_declaration(std::vector<id_token> &&rt, const id_token &&name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::unique_ptr<statement>> &&stmts) : rt(std::move(rt)), name(std::move(name)), params(std::move(params)), stmts(std::move(stmts)) {}

  private:
    void refine(scope &scp) const;

  private:
    std::vector<id_token> rt;
    id_token name;
    std::vector<std::pair<std::vector<id_token>, id_token>> params;
    std::vector<std::unique_ptr<statement>> stmts;
  };

  class predicate_declaration final
  {
    friend class compilation_unit;
    friend class class_declaration;

  public:
    predicate_declaration(const id_token &&name, std::vector<std::pair<std::vector<id_token>, id_token>> &&params, std::vector<std::vector<id_token>> &&base_predicates, std::vector<std::unique_ptr<statement>> &&body) : name(std::move(name)), params(std::move(params)), base_predicates(std::move(base_predicates)), body(std::move(body)) {}

  private:
    void declare(scope &scp) const;
    void refine(scope &scp) const;

  private:
    id_token name;
    std::vector<std::pair<std::vector<id_token>, id_token>> params;
    std::vector<std::vector<id_token>> base_predicates;
    std::vector<std::unique_ptr<statement>> body;
  };

  class class_declaration final : public type_declaration
  {
  public:
    class_declaration(const id_token &&name, std::vector<std::vector<id_token>> &&base_classes, std::vector<std::unique_ptr<field_declaration>> &&fields, std::vector<std::unique_ptr<constructor_declaration>> &&constructors, std::vector<std::unique_ptr<method_declaration>> &&methods, std::vector<std::unique_ptr<predicate_declaration>> &&predicates, std::vector<std::unique_ptr<type_declaration>> &&types) : name(std::move(name)), base_classes(std::move(base_classes)), fields(std::move(fields)), constructors(std::move(constructors)), methods(std::move(methods)), predicates(std::move(predicates)), types(std::move(types)) {}

  private:
    void declare(scope &scp) const override;
    void refine(scope &scp) const override;
    void refine_predicates(scope &scp) const override;

  private:
    id_token name;
    std::vector<std::vector<id_token>> base_classes;
    std::vector<std::unique_ptr<field_declaration>> fields;
    std::vector<std::unique_ptr<constructor_declaration>> constructors;
    std::vector<std::unique_ptr<method_declaration>> methods;
    std::vector<std::unique_ptr<predicate_declaration>> predicates;
    std::vector<std::unique_ptr<type_declaration>> types;
  };
} // namespace riddle
