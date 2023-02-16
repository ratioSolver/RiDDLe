#include "method.h"
#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT method::method(scope &scp, std::string name, std::vector<field_ptr> &args) : scope(scp), name(name)
    {
        if (auto tp = dynamic_cast<type *>(&scp))
            add_field(new field(*tp, "this", nullptr, true));
        for (auto &arg : args)
            add_field(std::move(arg));
    }
} // namespace riddle
