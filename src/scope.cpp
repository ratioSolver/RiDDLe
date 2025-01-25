#include "scope.hpp"
#include <stdexcept>

namespace riddle
{
    field::field(type &tp, std::string &&name, const utils::u_ptr<expression> &expr, bool synthetic) noexcept : tp(tp), name(std::move(name)), expr(expr), synthetic(synthetic) {}

    scope::scope(core &c, scope &parent, std::vector<utils::u_ptr<field>> &&args) : cr(c), parent(parent)
    {
        for (auto &arg : args)
            add_field(std::move(arg));
    }

    field &scope::get_field(std::string_view name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second;
        return parent.get_field(name);
    }

    void scope::add_field(utils::u_ptr<field> field)
    {
        std::string name = field->get_name();
        if (!fields.emplace(name, std::move(field)).second)
            throw std::invalid_argument("field " + name + " already exists");
    }
} // namespace riddle
