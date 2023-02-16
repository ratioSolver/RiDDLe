#pragma once

#include "scope.h"

namespace riddle
{
  class item;
  using expr = utils::c_ptr<item>;

  class type
  {
  public:
    RIDDLE_EXPORT type(core &cr, const std::string &name);
    virtual ~type() = default;

    const std::string &get_name() const { return name; }

    virtual expr new_instance() = 0;

    bool operator==(const type &other) const { return this == &other; }
    bool operator!=(const type &other) const { return this != &other; }

  protected:
    core &cr;

  private:
    std::string name;
  };

  using type_ptr = utils::u_ptr<type>;

  class bool_type final : public type
  {
  public:
    bool_type(core &cr);

    expr new_instance() override;
  };

  class int_type final : public type
  {
  public:
    int_type(core &cr);

    expr new_instance() override;
  };

  class real_type final : public type
  {
  public:
    real_type(core &cr);

    expr new_instance() override;
  };

  class complex_type : public scope, public type
  {
  public:
    RIDDLE_EXPORT complex_type(scope &scp, const std::string &name);
    virtual ~complex_type() = default;

    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;

    expr new_instance() override;

  private:
    std::map<std::string, type_ptr> types;
    std::map<std::string, std::vector<method_ptr>> methods;
  };
} // namespace riddle
