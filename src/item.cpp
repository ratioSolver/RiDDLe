#include "item.h"

namespace riddle
{
    item::item(type &tp) : tp(tp) {}

    complex_item::complex_item(type &tp, context &ctx) : item(tp), context(ctx) {}
} // namespace riddle