#include "method.h"
#include "type.h"
#include "item.h"
#include "core.h"

namespace riddle
{
    RIDDLE_EXPORT method::method(scope &scp, std::string name, std::vector<field_ptr> &&as, const std::vector<ast::statement_ptr> &body, type *ret_type) : scope(scp), ret_type(ret_type), name(name), body(body)
    {
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    RIDDLE_EXPORT expr method::call(expr &self, std::vector<expr> exprs)
    { // we create a new environment for the method..
        env ctx;
        if (auto c_env = dynamic_cast<env *>(self.operator->()))
            ctx = env(c_env);
        else
            throw std::runtime_error("invalid method call");

        // ..and we add the arguments to it
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (!args[i].get().get_type().is_assignable_from(exprs[i]->get_type()))
                throw std::runtime_error("invalid argument type");
            ctx.items.emplace(args[i].get().get_name(), std::move(exprs[i]));
        }

        // we execute the method body..
        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);

        // and we return the value of the return keyword if it exists..
        if (ctx.items.find(RETURN_KW) != ctx.items.end())
            return ctx.items.at(RETURN_KW);
        else
            return nullptr;
    }
} // namespace riddle
