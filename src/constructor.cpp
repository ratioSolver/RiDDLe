#include "constructor.h"
#include "type.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(complex_type &tp, std::vector<field_ptr> &as, std::vector<ast::statement_ptr> &body) : scope(tp), body(std::move(body))
    {
        add_field(new field(tp, THIS_KW, nullptr, true));
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    RIDDLE_EXPORT expr constructor::new_instance(std::vector<expr> &as)
    {
        complex_type &tp = static_cast<complex_type &>(get_scope());
        auto inst = tp.new_instance();
        call(inst, as);
        return inst;
    }

    RIDDLE_EXPORT void constructor::call(expr &self, std::vector<expr> exprs)
    {
        env ctx(static_cast<complex_item *>(self.operator->()));
        for (size_t i = 0; i < args.size(); ++i)
            ctx.items.emplace(args[i].get().get_name(), std::move(exprs[i]));

        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);
    }
} // namespace riddle
