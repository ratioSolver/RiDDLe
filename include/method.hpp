#pragma once

#include <vector>
#include "scope.hpp"

namespace riddle
{
  class statement;

  class method : public scope
  {
  public:
    method(std::shared_ptr<scope> parent, std::optional<std::reference_wrapper<type>> return_type, const std::string &name, std::vector<std::unique_ptr<field>> &&args, std::vector<std::unique_ptr<statement>> &&body);

    /**
     * @brief Get the return type of the method.
     *
     * @return std::shared_ptr<type> The return type of the method.
     */
    const std::optional<std::reference_wrapper<type>> &get_return_type() const { return return_type; }

    /**
     * @brief Get the name of the method.
     *
     * @return const std::string& The name of the method.
     */
    const std::string &get_name() const { return name; }

    /**
     * @brief Get the arguments of the method.
     *
     * @return const std::vector<std::reference_wrapper<field>>& The arguments of the method.
     */
    const std::vector<std::reference_wrapper<field>> &get_arguments() const { return arguments; }

    /**
     * @brief Invoke the method.
     *
     * @param ctx The context.
     * @param arguments The arguments.
     * @return std::shared_ptr<item> The result of the method.
     */
    std::shared_ptr<item> invoke(std::shared_ptr<env> &ctx, std::vector<std::shared_ptr<item>> &&args);

  private:
    std::optional<std::reference_wrapper<type>> return_type;
    std::string name;
    std::vector<std::reference_wrapper<field>> arguments;
    std::vector<std::unique_ptr<statement>> body;
  };
} // namespace riddle
