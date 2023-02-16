#include "item.h"
#include "type.h"

namespace riddle
{
    item::item(type &tp) : tp(tp) {}

    complex_item::complex_item(type &tp) : item(tp) {}
} // namespace riddle