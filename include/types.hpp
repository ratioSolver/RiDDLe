#pragma once

#include "type.hpp"
#include "timeline.hpp"
#include "flaw.hpp"

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

  class flaw_aware_component_type : public component_type
  {
  public:
    flaw_aware_component_type(scope &scp, std::string &&name) noexcept : component_type(scp, std::move(name)) {}
    virtual ~flaw_aware_component_type() = default;

    [[nodiscard]] virtual std::vector<std::shared_ptr<flaw>> get_flaws() noexcept = 0;
  };

  class state_variable : public flaw_aware_component_type, public timeline
  {
  public:
    state_variable(core &cr) noexcept;

    [[nodiscard]] virtual std::vector<std::shared_ptr<flaw>> get_flaws() noexcept override;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;

    virtual void created_atom(atom_expr atm) override;

    virtual std::shared_ptr<flaw> new_peak(std::vector<atom_expr> &&atms) noexcept = 0;
  };

  class reusable_resource : public flaw_aware_component_type, public timeline
  {
  public:
    reusable_resource(core &cr) noexcept;

    [[nodiscard]] virtual std::vector<std::shared_ptr<flaw>> get_flaws() noexcept override;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;

    virtual void created_atom(atom_expr atm) override;

    virtual std::shared_ptr<flaw> new_peak(std::vector<atom_expr> &&atms) noexcept = 0;

  private:
    std::unique_ptr<constructor_declaration> ctr;
    std::unique_ptr<predicate_declaration> use_pred;
  };

  class consumable_resource : public flaw_aware_component_type, public timeline
  {
  public:
    consumable_resource(core &cr) noexcept;

    [[nodiscard]] virtual std::vector<std::shared_ptr<flaw>> get_flaws() noexcept override;

    [[nodiscard]] virtual json::json extract() const override;

  private:
    void created_predicate(predicate &pred) noexcept override;

    virtual void created_atom(atom_expr atm) override;

    virtual std::shared_ptr<flaw> new_overproduction(std::vector<atom_expr> &&prod_atms, std::vector<atom_expr> &&cons_atms) noexcept = 0;
    virtual std::shared_ptr<flaw> new_overconsumption(std::vector<atom_expr> &&cons_atms, std::vector<atom_expr> &&prod_atms) noexcept = 0;

  private:
    std::unique_ptr<constructor_declaration> ctr;
    std::unique_ptr<predicate_declaration> prod_pred;
    std::unique_ptr<predicate_declaration> cons_pred;
  };

  [[nodiscard]] std::vector<std::shared_ptr<resolver>> causes_from_atoms(const std::vector<atom_expr> &atms) noexcept;
} // namespace riddle