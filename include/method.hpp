#pragma once

#include "scope.hpp"
#include "statement.hpp"
#include <optional>

namespace riddle
{
  class method : public scope
  {
  public:
    method(scope &scp, std::optional<utils::ref_wrapper<type>> return_type, std::string_view name, std::vector<utils::u_ptr<field>> &&args = {}, const std::vector<utils::u_ptr<statement>> &body = {}) noexcept;

    /**
     * @brief Retrieves the name of the method.
     *
     * This function returns a constant reference to the name of the method.
     *
     * @return const std::string& A constant reference to the name of the method.
     */
    [[nodiscard]] const std::string &get_name() const noexcept { return name; }

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
    [[nodiscard]] expr invoke(env &ctx, std::vector<expr> &&args) const;

  private:
    const std::optional<utils::ref_wrapper<type>> return_type; // The return type of the method.
    const std::string name;                                    // The name of the method.
    std::vector<std::string> args;                             // The names of the arguments.
    const std::vector<utils::u_ptr<statement>> &body;          // The body of the method.
  };
} // namespace riddle
