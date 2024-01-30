#include "conjunction.hpp"

namespace riddle
{
    conjunction::conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx, const utils::rational &cst) : scope(parent->get_core(), parent), ctx(ctx), cst(cst) {}
} // namespace riddle