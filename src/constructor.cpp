#include "constructor.h"
#include "type.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(complex_type &tp, std::vector<field_ptr> as, const std::vector<ast::statement_ptr> &body) : scope(tp), body(body)
    { // we add the "this" field..
        add_field(new field(tp, THIS_KW, nullptr, true));
        // ..and we add the arguments..
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    RIDDLE_EXPORT void constructor::call(expr &self, std::vector<expr> exprs)
    { // we create a new environment for the constructor..
        env ctx(static_cast<complex_item *>(self.operator->()));
        for (size_t i = 0; i < args.size(); ++i)
        { // ..and we add the arguments to it
            if (!args[i].get().get_type().is_assignable_from(exprs[i]->get_type()))
                throw std::runtime_error("invalid argument type");
            ctx.items.emplace(args[i].get().get_name(), std::move(exprs[i]));
        }

        // we execute the constructor body..
        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);
    }
} // namespace riddle
