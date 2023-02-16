#pragma once

#include "scope.h"

namespace riddle
{
  class item;
  using expr = utils::c_ptr<item>;

  class type : public scope
  {
  public:
    RIDDLE_EXPORT type(scope &scp, const std::string &name);
    virtual ~type() = default;

    const std::string &get_name() const { return name; }

    virtual expr new_instance() = 0;

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
} // namespace riddle
