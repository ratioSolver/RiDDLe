#pragma once

#include "scope.hpp"

namespace riddle
{
  class type : public scope
  {
  public:
    type(std::shared_ptr<scope> parent, const std::string &name);
    virtual ~type() = default;

    /**
     * @brief Get the name of the type.
     *
     * @return const std::string& The name of the type.
     */
    const std::string &get_name() const { return name; }

  private:
    std::string name;
  };
} // namespace riddle
