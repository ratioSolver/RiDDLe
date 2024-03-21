#include "conjunction.hpp"
#include "statement.hpp"

namespace riddle
{
    conjunction::conjunction(scope &parent, std::shared_ptr<env> ctx, const conjunction_statement &conj) : scope(parent.get_core(), parent), ctx(ctx), conj(conj) {}

    void conjunction::execute() { conj.execute(*this, ctx); }
} // namespace riddle