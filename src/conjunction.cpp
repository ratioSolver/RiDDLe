#include "conjunction.h"
#include "item.h"

namespace riddle
{
    conjunction::conjunction(scope &scp, env ctx, const utils::rational cst, const std::vector<ast::statement_ptr> &body) : scope(scp), ctx(ctx), cost(cst), body(body) {}

    void conjunction::execute()
    { // execute the body of the conjunction..
        for (auto &stmt : body)
            stmt->execute(*this, ctx);
    }
} // namespace riddle
