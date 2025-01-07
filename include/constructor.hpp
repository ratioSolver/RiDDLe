#pragma once

#include "scope.hpp"
#include "statement.hpp"

namespace riddle
{
  class constructor final : public scope
  {
  public:
    constructor(scope &scp, std::vector<std::unique_ptr<field>> &&args = {}, const std::vector<std::pair<id_token, std::vector<std::unique_ptr<expression>>>> &inits = {}, const std::vector<std::unique_ptr<statement>> &body = {}) noexcept;
    constructor(const constructor &) = delete;

    /**
     * @brief Retrieves the arguments.
     *
     * This function returns a constant reference to the vector of arguments.
     *
     * @return const std::vector<std::string>& A constant reference to the vector of arguments.
     */
    [[nodiscard]] const std::vector<std::string> &get_args() const noexcept { return args; }

    void invoke(std::shared_ptr<component> self, std::vector<std::shared_ptr<item>> &&args) const;

  private:
    std::vector<std::string> args;                                                           // The names of the arguments.
    const std::vector<std::pair<id_token, std::vector<std::unique_ptr<expression>>>> &inits; // The initializations.
    const std::vector<std::unique_ptr<statement>> &body;                                     // The body of the constructor.
  };
} // namespace riddle
