#include "type.h"
#include "core.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT type::type(scope &scp, const std::string &name) : scope(scp), name(name) {}

    bool_type::bool_type(core &cr) : type(cr, "bool") {}

    expr bool_type::new_instance() { return get_core().new_bool(); }

    int_type::int_type(core &cr) : type(cr, "int") {}

    expr int_type::new_instance() { return get_core().new_int(); }

    real_type::real_type(core &cr) : type(cr, "real") {}

    expr real_type::new_instance() { return get_core().new_real(); }
} // namespace riddle
