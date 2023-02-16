#include "scope.h"

namespace riddle
{
    RIDDLE_EXPORT scope::scope(scope &scp) : scp(scp) {}

    RIDDLE_EXPORT field &scope::get_field(const std::string &name)
    {
        auto it = fields.find(name);
        if (it != fields.end())
            return *it->second;
        else
            return scp.get_field(name);
    }
} // namespace riddle