#include "method.h"
#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT method::method(scope &scp, std::string name, std::vector<field_ptr> &as, std::vector<ast::statement_ptr> &body) : scope(scp), name(name), body(std::move(body))
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
        throw std::runtime_error("method `" + name + "` not implemented");
    }
} // namespace riddle
