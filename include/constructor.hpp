#pragma once

#include "scope.hpp"
#include "statement.hpp"

namespace riddle
{
  class constructor : public scope
  {
  public:
    constructor(scope &scp, std::vector<std::unique_ptr<field>> &&args = {}, std::vector<std::unique_ptr<statement>> &&body = {}) noexcept;

    /**
     * @brief Retrieves the arguments.
     *
     * This function returns a constant reference to the vector of arguments.
     *
     * @return const std::vector<std::string>& A constant reference to the vector of arguments.
     */
    [[nodiscard]] const std::vector<std::string> &get_args() const noexcept { return args; }

    /**
     * @brief Invokes a function with the provided arguments.
     *
     * This function takes a vector of shared pointers to `item` objects as arguments
     * and returns a shared pointer to an `item` object.
     *
     * @param args A vector of shared pointers to `item` objects, passed as an rvalue reference.
     * @return std::shared_ptr<item> A shared pointer to the resulting `item` object.
     */
    [[nodiscard]] std::shared_ptr<item> invoke(std::vector<std::shared_ptr<item>> &&args) const;

  private:
    std::vector<std::string> args;                      // The names of the arguments.
    const std::vector<std::unique_ptr<statement>> body; // The body of the constructor.
  };
} // namespace riddle
