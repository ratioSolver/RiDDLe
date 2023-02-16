#pragma once

#include "type.h"
#include "item.h"
#include "method.h"

namespace riddle
{
  class core : public scope, public context
  {
  public:
    RIDDLE_EXPORT core();
    virtual ~core() = default;

    core &get_core() override { return *this; }

    RIDDLE_EXPORT virtual expr new_bool();
    RIDDLE_EXPORT virtual expr new_bool(bool value);

    RIDDLE_EXPORT virtual expr new_int();
    RIDDLE_EXPORT virtual expr new_int(utils::I value);

    RIDDLE_EXPORT virtual expr new_real();
    RIDDLE_EXPORT virtual expr new_real(utils::rational value);

    RIDDLE_EXPORT type &get_type(const std::string &name) override;
    RIDDLE_EXPORT method &get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args) override;

    RIDDLE_EXPORT expr &get(const std::string &name) override;

  private:
    std::map<std::string, type_ptr> types;
    std::map<std::string, std::vector<method_ptr>> methods;
  };
} // namespace riddle