#include "constructor.h"
#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(type &tp, std::vector<field_ptr> &args) : scope(tp)
    {
        for (auto &arg : args)
            add_field(std::move(arg));
    }
} // namespace riddle
