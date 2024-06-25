#include "conjunction.hpp"
#include "statement.hpp"
#include "core.hpp"

namespace riddle
{
    conjunction::conjunction(scope &parent, std::shared_ptr<env> ctx, const conjunction_statement &conj) : scope(parent.get_core(), parent), ctx(ctx), conj(conj) {}

    void conjunction::execute() { conj.execute(*this, ctx); }

    std::shared_ptr<arith_item> conjunction::compute_cost() const noexcept
    {
        if (conj.get_cost())
            return std::static_pointer_cast<arith_item>(conj.get_cost()->evaluate(*this, ctx));
        else
            return get_core().new_real(utils::rational::one);
    }
} // namespace riddle