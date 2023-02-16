#include "item.h"

namespace riddle
{
    item::item(type &tp) : tp(tp) {}

    complex_item::complex_item(type &tp, env &ctx) : item(tp), env(ctx) {}
} // namespace riddle