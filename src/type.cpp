#include "type.hpp"
#include "core.hpp"
#include "constructor.hpp"
#include "declaration.hpp"

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

    typedef_type::typedef_type(core &c, const std::string &name, type &base_type, std::unique_ptr<expression> &&value) : type(c, name), base_type(base_type), value(std::move(value)) {}
    std::shared_ptr<item> typedef_type::new_instance()
    {
        // we create a new environment for the expression evaluation..
        env ctx(scp.get_core());
        return value->evaluate(scp, ctx);
    }

    enum_type::enum_type(core &c, const std::string &name, std::vector<std::shared_ptr<item>> &&values) : type(c, name), values(std::move(values)) {}
    std::vector<std::shared_ptr<item>> enum_type::get_values() const
    {
        std::vector<std::shared_ptr<item>> vals;
        for (const auto &val : values)
            vals.push_back(val);
        for (const auto &e : enums)
            for (const auto &val : e.get().get_values())
                vals.push_back(val);
        return vals;
    }
    std::shared_ptr<item> enum_type::new_instance() { return scp.get_core().new_enum(*this, get_values()); }

    component_type::component_type(std::shared_ptr<scope> parent, const std::string &name) : type(*parent, name), scope(parent->get_core(), parent) {}

    predicate::predicate(std::shared_ptr<scope> parent, const std::string &name, std::vector<std::unique_ptr<field>> &&args, std::vector<std::unique_ptr<statement>> &&body) : type(*parent, name), scope(parent->get_core(), parent), body(std::move(body)) {}
} // namespace riddle