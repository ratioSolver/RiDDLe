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

    /**
     * @brief Invokes a method with the given environment context and arguments.
     *
     * This function executes a method within the provided environment context (`ctx`)
     * using the specified arguments (`args`). The method is expected to return a
     * shared pointer to an `item` object.
     *
     * @param ctx The environment context in which the method is invoked.
     * @param args A vector of shared pointers to `item` objects representing the arguments.
     * @return A shared pointer to an `item` object resulting from the method invocation.
     */
    [[nodiscard]] std::shared_ptr<item> invoke(env &ctx, std::vector<std::shared_ptr<item>> &&args) const;

  private:
    std::vector<std::string> args;                      // The names of the arguments.
    const std::vector<std::unique_ptr<statement>> body; // The body of the method.
  };
} // namespace riddle
