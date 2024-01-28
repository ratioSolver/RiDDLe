#pragma once

#include <vector>
#include "scope.hpp"

namespace riddle
{
  class constructor : public scope
  {
  public:
    constructor(std::shared_ptr<scope> parent, std::vector<std::unique_ptr<field>> &&args);

    /**
     * @brief Get the arguments of the constructor.
     *
     * @return const std::vector<std::unique_ptr<field>>& The arguments of the constructor.
     */
    const std::vector<std::unique_ptr<field>> &get_args() const { return args; }

  private:
    std::vector<std::unique_ptr<field>> args;
  };
} // namespace riddle
