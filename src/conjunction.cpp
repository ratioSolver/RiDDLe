#include "conjunction.hpp"
#include "statement.hpp"

namespace riddle
{
    conjunction::conjunction(scope &parent, std::shared_ptr<env> ctx, const conjunction_statement &conj) : scope(parent.get_core(), parent), ctx(ctx), conj(conj) {}

    void conjunction::execute() { conj.execute(*this, ctx); }

    std::shared_ptr<arith_item> conjunction::compute_cost() const noexcept { return std::static_pointer_cast<arith_item>(conj.get_cost()->evaluate(*this, ctx)); }
} // namespace riddle