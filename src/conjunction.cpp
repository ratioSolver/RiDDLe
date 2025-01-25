#include "conjunction.hpp"

namespace riddle
{
    conjunction::conjunction(const scope &scp, env &&ctx, const utils::rational cst, const std::vector<utils::u_ptr<statement>> &body) noexcept : scp(scp), ctx(std::move(ctx)), cst(cst), body(body) {}

    void conjunction::execute()
    {
        for (auto &stmt : body)
            stmt->execute(scp, ctx);
    }
} // namespace riddle