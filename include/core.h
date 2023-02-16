#pragma once

#include "scope.h"
#include "env.h"

namespace riddle
{
  class core : public scope, public env
  {
  public:
    RIDDLE_EXPORT core();
    virtual ~core() = default;

    core &get_core() override { return *this; }

    RIDDLE_EXPORT virtual expr new_bool();
    RIDDLE_EXPORT virtual expr new_bool(bool value);

    RIDDLE_EXPORT virtual expr new_int();
    RIDDLE_EXPORT virtual expr new_int(utils::I value);

    RIDDLE_EXPORT virtual expr new_real();
    RIDDLE_EXPORT virtual expr new_real(utils::rational value);

    RIDDLE_EXPORT expr get(const std::string &name) const override;

  private:
    std::map<std::string, expr> items;
  };
} // namespace riddle