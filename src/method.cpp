#include "method.h"
#include "type.h"
#include "item.h"
#include "core.h"

namespace riddle
{
    RIDDLE_EXPORT method::method(scope &scp, std::string name, std::vector<field_ptr> &&as, const std::vector<ast::statement_ptr> &body, type *ret_type) : scope(scp), ret_type(ret_type), name(name), body(body)
    {
        if (auto tp = dynamic_cast<type *>(&scp))
            add_field(new field(*tp, THIS_KW, nullptr, true));
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    RIDDLE_EXPORT expr method::call(expr &self, std::vector<expr> exprs)
    {
        env ctx;
        if (auto ci = dynamic_cast<complex_item *>(self.operator->()))
            ctx = env(ci);
        else if (auto cr = dynamic_cast<core *>(self.operator->()))
            ctx = env(cr);
        else
            throw std::runtime_error("invalid method call");

        for (size_t i = 0; i < args.size(); ++i)
            ctx.items.emplace(args[i].get().get_name(), std::move(exprs[i]));

        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);

        if (ctx.items.find(RETURN_KW) != ctx.items.end())
            return ctx.items.at(RETURN_KW);
        else
            return nullptr;
    }
} // namespace riddle
