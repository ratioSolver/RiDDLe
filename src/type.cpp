#include "type.h"
#include "core.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT type::type(scope &scp, const std::string &name) : scope(scp), name(name) {}

    RIDDLE_EXPORT bool_type::bool_type(core &cr) : type(cr, "bool") {}

    RIDDLE_EXPORT expr bool_type::new_instance() { return get_core().new_bool(); }

    RIDDLE_EXPORT int_type::int_type(core &cr) : type(cr, "int") {}

    RIDDLE_EXPORT expr int_type::new_instance() { return get_core().new_int(); }

    RIDDLE_EXPORT real_type::real_type(core &cr) : type(cr, "real") {}

    RIDDLE_EXPORT expr real_type::new_instance() { return get_core().new_real(); }
} // namespace riddle
