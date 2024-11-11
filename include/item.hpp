#pragma once

#include "env.hpp"
#include "enum.hpp"
#include "lit.hpp"
#include "lin.hpp"

namespace riddle
{
  class type;
  class bool_type;
  class int_type;
  class real_type;
  class time_type;
  class string_type;
  class component_type;
  class predicate;
  class bool_item;

  constexpr std::string_view TAU_NAME = "tau";

  class item : public utils::enum_val
  {
  public:
    item(type &t);
    virtual ~item() = default;

    /**
     * @brief Get the type of the item.
     *
     * @return type& The type of the item.
     */
    [[nodiscard]] type &get_type() const { return tp; }

    friend std::shared_ptr<bool_item> operator==(std::shared_ptr<item> lhs, std::shared_ptr<item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator!=(std::shared_ptr<item> lhs, std::shared_ptr<item> rhs) noexcept;

  private:
    type &tp;
  };

  class bool_item : public item
  {
  public:
    bool_item(bool_type &t, const utils::lit &l);

    [[nodiscard]] utils::lit &get_value() { return value; }
    [[nodiscard]] const utils::lit &get_value() const { return value; }

    friend std::shared_ptr<bool_item> operator&&(std::shared_ptr<bool_item> lhs, std::shared_ptr<bool_item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator||(std::shared_ptr<bool_item> lhs, std::shared_ptr<bool_item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator!(std::shared_ptr<bool_item> lhs) noexcept;

  private:
    utils::lit value;
  };

  class arith_item : public item
  {
  public:
    arith_item(int_type &t, const utils::lin &l);
    arith_item(real_type &t, const utils::lin &l);
    arith_item(time_type &t, const utils::lin &l);

    [[nodiscard]] utils::lin &get_value() { return value; }
    [[nodiscard]] const utils::lin &get_value() const { return value; }

    friend std::shared_ptr<arith_item> operator+(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<arith_item> operator-(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<arith_item> operator*(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<arith_item> operator/(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;

    friend std::shared_ptr<bool_item> operator<(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator<=(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator>(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;
    friend std::shared_ptr<bool_item> operator>=(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept;

  private:
    utils::lin value;
  };

  class string_item : public item
  {
  public:
    string_item(string_type &t, const std::string &s = "");

    [[nodiscard]] std::string &get_value() { return value; }
    [[nodiscard]] const std::string &get_value() const { return value; }

  private:
    std::string value;
  };

  class enum_item : public item, public env
  {
  public:
    enum_item(type &t, utils::var v);

    [[nodiscard]] std::shared_ptr<item> get(std::string_view name) override;

    [[nodiscard]] utils::var &get_value() { return value; }
    [[nodiscard]] const utils::var &get_value() const { return value; }

  private:
    utils::var value;
  };

  class component : public item, public env
  {
  public:
    component(component_type &t, std::shared_ptr<env> parent = nullptr);
    virtual ~component() = default;
  };

  class atom : public item, public env
  {
  public:
    atom(predicate &t, bool is_fact, const utils::lit &sigma, std::map<std::string, std::shared_ptr<item>, std::less<>> &&args = {});
    virtual ~atom() = default;

    /**
     * @brief Checks if the item is a fact.
     *
     * @return true if the item is a fact, false otherwise.
     */
    [[nodiscard]] bool is_fact() const { return fact; }
    /**
     * @brief Get the status of the atom as a literal.
     *
     * @return const utils::lit& the status of the atom.
     * @note the literal assumes the following values:
     * - `True` if the atom is `Active`.
     * - `False` if the atom is `Unified`.
     * - `Undefined` if the atom is `Inactive`.
     */
    [[nodiscard]] utils::lit &get_sigma() { return sigma; }
    /**
     * @brief Get the status of the atom as a literal.
     *
     * @return const utils::lit& the status of the atom.
     * @note the literal assumes the following values:
     * - `True` if the atom is `Active`.
     * - `False` if the atom is `Unified`.
     * - `Undefined` if the atom is `Inactive`.
     */
    [[nodiscard]] const utils::lit &get_sigma() const { return sigma; }

  private:
    bool fact;        // whether the atom is a fact or a goal..
    utils::lit sigma; // the literal indicating the status of the atom (i.e., true if active, false if unified, undefined if inactive)..
  };

  /**
   * @brief Gets the unique identifier of the given item.
   *
   * @param f the item to get the unique identifier of.
   * @return uintptr_t the unique identifier of the given item.
   */
  inline uintptr_t get_id(const item &itm) noexcept { return reinterpret_cast<uintptr_t>(&itm); }
} // namespace riddle
