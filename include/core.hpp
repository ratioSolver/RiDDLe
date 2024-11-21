#pragma once

#include "env.hpp"

namespace riddle
{
  /**
   * @class core core.hpp "include/core.hpp"
   * @brief The core environment class.
   */
  class core : public env
  {
  public:
    core() noexcept;
    virtual ~core() = default;

    [[nodiscard]] std::shared_ptr<item> get(std::string_view name) noexcept override;
  };
} // namespace riddle
