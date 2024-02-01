#include "conjunction.hpp"
#include "statement.hpp"

namespace riddle
{
    conjunction::conjunction(std::shared_ptr<scope> parent, std::shared_ptr<env> ctx, const utils::rational &cst, std::vector<std::unique_ptr<statement>> &&body) : scope(parent->get_core(), parent), ctx(ctx), cst(cst), body(std::move(body)) {}

    void conjunction::execute()
    { // execute the body of the conjunction..
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);
    }
} // namespace riddle