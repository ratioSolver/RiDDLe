#include "type.hpp"
#include "core.hpp"

namespace riddle
{
    type::type(scope &scp, const std::string &name, bool primitive) : scp(scp), name(name), primitive(primitive) {}

    bool_type::bool_type(core &c) : type(c, "bool", true) {}
    int_type::int_type(core &c) : type(c, "int", true) {}
    real_type::real_type(core &c) : type(c, "real", true) {}
    time_type::time_type(core &c) : type(c, "time", true) {}
    string_type::string_type(core &c) : type(c, "string", true) {}

    component_type::component_type(std::shared_ptr<scope> parent, const std::string &name) : type(*parent, name), scope(parent->get_core(), parent) {}

    predicate::predicate(std::shared_ptr<scope> parent, const std::string &name) : type(*parent, name), scope(parent->get_core(), parent) {}
} // namespace riddle