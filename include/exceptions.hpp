#pragma once

#include <stdexcept>

namespace riddle
{
  class inconsistency_exception : public std::exception
  {
    const char *what() const noexcept override { return "inconsistency detected"; }
  };

  class unsolvable_exception : public std::exception
  {
    const char *what() const noexcept override { return "the problem is unsolvable.."; }
  };
} // namespace riddle