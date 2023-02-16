#include "method.h"

namespace riddle
{
    RIDDLE_EXPORT method::method(scope &scp, std::string name, std::vector<field_ptr> &args) : scope(scp), name(name)
    {
        for (auto &arg : args)
            add_field(std::move(arg));
    }
} // namespace riddle
