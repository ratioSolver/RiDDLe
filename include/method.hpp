#pragma once

#include "scope.hpp"
#include "statement.hpp"

namespace riddle
{
  class method : public scope
  {
  public:
    method(scope &scp, std::vector<std::unique_ptr<field>> &&args = {}, std::vector<std::unique_ptr<statement>> &&body = {}) noexcept;

    /**
     * @brief Retrieves the arguments.
     *
     * This function returns a constant reference to the vector of arguments.
     *
     * @return const std::vector<std::string>& A constant reference to the vector of arguments.
     */
    [[nodiscard]] const std::vector<std::string> &get_args() const noexcept { return args; }

  private:
    std::vector<std::string> args;                      // The names of the arguments.
    const std::vector<std::unique_ptr<statement>> body; // The body of the method.
  };
} // namespace riddle
