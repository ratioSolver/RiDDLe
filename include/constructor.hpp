#pragma once

#include <vector>
#include "scope.hpp"

namespace riddle
{
  class init_element;
  class statement;
  class item;

  class constructor : public scope
  {
  public:
    constructor(scope &parent, std::vector<std::unique_ptr<field>> &&args, const std::vector<init_element> &inits, const std::vector<std::unique_ptr<statement>> &body);

    /**
     * @brief Get the arguments of the constructor.
     *
     * @return const std::vector<std::unique_ptr<field>>& The arguments of the constructor.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<field>> &get_args() const { return args; }

    /**
     * @brief Invoke the constructor.
     *
     * @param args The arguments.
     * @return std::shared_ptr<item> A new instance of the type.
     */
    std::shared_ptr<item> invoke(std::vector<std::shared_ptr<item>> &&args);

  private:
    std::vector<std::unique_ptr<field>> args;            // the arguments of the constructor..
    const std::vector<init_element> &inits;              // the initializations of the constructor..
    const std::vector<std::unique_ptr<statement>> &body; // the body of the constructor..
  };
} // namespace riddle
