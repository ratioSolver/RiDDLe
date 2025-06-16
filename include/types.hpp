#pragma once

#include "type.hpp"

namespace riddle
{
  constexpr const char *state_variable_kw = "StateVariable";
  constexpr const char *reusable_resource_kw = "ReusableResource";
  constexpr const char *reusable_resource_capacity_kw = "capacity";
  constexpr const char *reusable_resource_use_predicate_kw = "Use";
  constexpr const char *reusable_resource_amount_kw = "amount";
  constexpr const char *consumable_resource_kw = "ConsumableResource";
  constexpr const char *consumable_resource_capacity_kw = "capacity";
  constexpr const char *consumable_resource_initial_amount_kw = "initial_amount";
  constexpr const char *consumable_resource_produce_predicate_kw = "Produce";
  constexpr const char *consumable_resource_consume_predicate_kw = "Consume";
  constexpr const char *consumable_resource_amount_kw = "amount";

  class core;
  class constructor_declaration;

  class state_variable : public component_type, public timeline
  {
  public:
    state_variable(core &cr) noexcept;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;
  };

  class reusable_resource : public component_type, public timeline
  {
  public:
    reusable_resource(core &cr) noexcept;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;

  private:
    std::unique_ptr<constructor_declaration> ctr;
    std::unique_ptr<predicate_declaration> use_pred;
  };

  class consumable_resource : public component_type, public timeline
  {
  public:
    consumable_resource(core &cr) noexcept;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;

  private:
    std::unique_ptr<constructor_declaration> ctr;
    std::unique_ptr<predicate_declaration> prod_pred;
    std::unique_ptr<predicate_declaration> cons_pred;
  };
} // namespace riddle