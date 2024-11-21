#include "item.hpp"
#include "type.hpp"

namespace riddle
{
    bool_item::bool_item(bool_type &tp, const utils::lit &l) : item(tp), value(l) {}
} // namespace riddle