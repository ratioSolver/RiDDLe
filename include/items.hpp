#pragma once

#include "term.hpp"
#include "lit.hpp"
#include "lin.hpp"
#include <unordered_map>

namespace riddle
{
  constexpr const char *origin_kw = "origin";
  constexpr const char *horizon_kw = "horizon";

  class bool_item : public bool_term
  {
  public:
    bool_item(bool_type &tp, utils::lit &&expr) noexcept;

    [[nodiscard]] const utils::lit &get_lit() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    const utils::lit expr;
  };

  class arith_item : public arith_term
  {
  public:
    arith_item(int_type &tp, utils::lin &&expr) noexcept;
    arith_item(real_type &tp, utils::lin &&expr) noexcept;
    arith_item(time_type &tp, utils::lin &&expr) noexcept;

    [[nodiscard]] const utils::lin &get_lin() const noexcept { return expr; }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    const utils::lin expr;
  };

  class string_item : public string_term
  {
  public:
    string_item(string_type &tp, std::string &&expr) noexcept;

    [[nodiscard]] const std::string &get_string() const noexcept { return expr; }

  private:
    const std::string expr;
  };

  class enum_item : public enum_term
  {
  public:
    enum_item(component_type &tp, std::vector<utils::ref_wrapper<utils::enum_val>> &&values, std::vector<utils::lit> &&lits) noexcept;

    [[nodiscard]] bool has_lit(const utils::enum_val &val) const noexcept { return domain.find(&val) != domain.end(); }

    [[nodiscard]] const utils::lit &get_lit(const utils::enum_val &val) const noexcept { return domain.at(&val); }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    std::unordered_map<const utils::enum_val *, const utils::lit> domain;
  };

  class atom : public riddle::atom_term
  {
  public:
    atom(riddle::predicate &pred, bool is_fact, std::map<std::string, utils::s_ptr<riddle::term>, std::less<>> &&args, utils::lit &&sigma) noexcept;

    [[nodiscard]] const utils::lit &get_sigma() const noexcept { return sigma; }

    [[nodiscard]] virtual json::json to_json() const noexcept override;

  private:
    const utils::lit sigma;
  };

  [[nodiscard]] bool is_arith(const type &tp) noexcept;
  [[nodiscard]] bool is_arith(expr xpr) noexcept;
} // namespace riddle
