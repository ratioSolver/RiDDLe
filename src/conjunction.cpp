#include "conjunction.hpp"

namespace riddle
{
    conjunction::conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx) : scope(parent->get_core(), parent), ctx(ctx) {}
} // namespace riddle