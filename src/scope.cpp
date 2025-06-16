#include "scope.hpp"
#include <stdexcept>

namespace riddle
{
    field::field(type &tp, std::string &&name, const std::unique_ptr<expression> &expr) noexcept : tp(tp), name(std::move(name)), expr(expr) {}

    scope::scope(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args) : cr(c), parent(parent)
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

    void scope::add_field(std::unique_ptr<field> field)
    {
        std::string name = field->get_name();
        if (!fields.emplace(name, std::move(field)).second)
            throw std::invalid_argument("field `" + name + "` already exists");
    }
} // namespace riddle
