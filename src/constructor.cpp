#include "constructor.h"
#include "type.h"

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(type &tp) : scope(tp) {}
} // namespace riddle
