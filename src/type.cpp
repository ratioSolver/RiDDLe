#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT type::type(scope &scp, const std::string &name) : scope(scp), name(name) {}
} // namespace riddle
