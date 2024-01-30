#include "type.hpp"
#include "core.hpp"
#include "constructor.hpp"

namespace riddle
{
    type::type(scope &scp, const std::string &name, bool primitive) : scp(scp), name(name), primitive(primitive) {}

    bool_type::bool_type(core &c) : type(c, "bool", true) {}
    std::shared_ptr<item> bool_type::new_instance() { return scp.get_core().new_bool(); }

    int_type::int_type(core &c) : type(c, "int", true) {}
    std::shared_ptr<item> int_type::new_instance() { return scp.get_core().new_int(); }

    real_type::real_type(core &c) : type(c, "real", true) {}
    std::shared_ptr<item> real_type::new_instance() { return scp.get_core().new_real(); }

    time_type::time_type(core &c) : type(c, "time", true) {}
    std::shared_ptr<item> time_type::new_instance() { return scp.get_core().new_time(); }

    string_type::string_type(core &c) : type(c, "string", true) {}
    std::shared_ptr<item> string_type::new_instance() { return scp.get_core().new_string(); }

    component_type::component_type(std::shared_ptr<scope> parent, const std::string &name) : type(*parent, name), scope(parent->get_core(), parent) {}

    predicate::predicate(std::shared_ptr<scope> parent, const std::string &name) : type(*parent, name), scope(parent->get_core(), parent) {}
} // namespace riddle