#pragma once

#include <vector>
#include "scope.hpp"

namespace riddle
{
  class method : public scope
  {
  public:
    method(std::shared_ptr<scope> parent, std::shared_ptr<type> return_type, const std::string &name, std::vector<std::unique_ptr<field>> &&args);

    /**
     * @brief Get the return type of the method.
     *
     * @return std::shared_ptr<type> The return type of the method.
     */
    std::shared_ptr<type> get_return_type() const { return return_type; }

    /**
     * @brief Get the name of the method.
     *
     * @return const std::string& The name of the method.
     */
    const std::string &get_name() const { return name; }

    /**
     * @brief Get the arguments of the method.
     *
     * @return const std::vector<std::unique_ptr<field>>& The arguments of the method.
     */
    const std::vector<std::unique_ptr<field>> &get_args() const { return args; }

  private:
    std::shared_ptr<type> return_type;
    std::string name;
    std::vector<std::unique_ptr<field>> args;
  };
} // namespace riddle
