#pragma once

#include "scope.h"
#include <map>

namespace riddle
{
  class type;
  class complex_type;
  class predicate;

  class item : virtual public utils::countable
  {
  public:
    item(type &tp);
    virtual ~item() = default;

    type &get_type() { return tp; }
    const type &get_type() const { return tp; }

  private:
    type &tp;
  };

  class complex_item : public item, public env
  {
  public:
    complex_item(type &tp);
    virtual ~complex_item() = default;

    RIDDLE_EXPORT expr &get(const std::string &name) override;
  };

  inline uintptr_t get_id(const item &itm) noexcept { return reinterpret_cast<uintptr_t>(&itm); }
} // namespace riddle
