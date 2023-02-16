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

    RIDDLE_EXPORT expr get(const std::string &name) const override;

  private:
    std::map<std::string, expr> items;
  };
} // namespace riddle