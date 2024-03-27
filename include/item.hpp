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
  class predicate;

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

  private:
    type &tp;
  };

  class bool_item : public item
  {
  public:
    bool_item(bool_type &t, const utils::lit &l);

    [[nodiscard]] utils::lit &get_value() { return value; }
    [[nodiscard]] const utils::lit &get_value() const { return value; }

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
    enum_item(type &t, VARIABLE_TYPE v);

    [[nodiscard]] std::shared_ptr<item> get(const std::string &name) override;

    [[nodiscard]] VARIABLE_TYPE &get_value() { return value; }
    [[nodiscard]] const VARIABLE_TYPE &get_value() const { return value; }

  private:
    VARIABLE_TYPE value;
  };

  class component : public item, public env
  {
  public:
    component(type &t, std::shared_ptr<env> parent = nullptr);
    virtual ~component() = default;
  };

  class atom : public item, public env
  {
  public:
    atom(predicate &t, bool is_fact, std::map<std::string, std::shared_ptr<item>> &&args = {});
    virtual ~atom() = default;

    [[nodiscard]] bool is_fact() const { return fact; }

  private:
    bool fact; // whether the atom is a fact or a goal..
  };
} // namespace riddle
