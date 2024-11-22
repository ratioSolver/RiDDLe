#include "conjunction.hpp"

namespace riddle
{
    conjunction::conjunction(core &c, scope &parent, env &&ctx) noexcept : scope(c, parent), ctx(std::move(ctx)) {}
} // namespace riddle