#include "constructor.h"
#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(complex_type &tp, std::vector<field_ptr> &as, std::vector<ast::statement_ptr> &body) : scope(tp), body(std::move(body))
    {
        add_field(new field(tp, "this", nullptr, true));
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }
} // namespace riddle
