#include "conjunction.hpp"
#include "statement.hpp"

namespace riddle
{
    conjunction::conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx, const conjunction_statement &conj) : scope(parent->get_core(), parent), ctx(ctx), conj(conj) {}

    void conjunction::execute()
    {
        auto scp = shared_from_this();
        conj.execute(scp, ctx);
    }
} // namespace riddle