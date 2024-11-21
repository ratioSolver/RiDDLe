#pragma once

#include "env.hpp"

namespace riddle
{
  class core : public env
  {
  public:
    core();
    virtual ~core() = default;

    std::optional<std::reference_wrapper<item>> get(std::string_view name) override;
  };
} // namespace riddle
