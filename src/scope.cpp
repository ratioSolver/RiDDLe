#include "scope.hpp"

namespace riddle
{
    scope::scope(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args) : cr(c), parent(parent)
    {
        for (auto &arg : args)
            add_field(std::move(arg));
    }

    std::optional<std::reference_wrapper<field>> scope::get_field(std::string_view name) const noexcept
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second.get();
        return parent.get_field(name);
    }

    void scope::add_field(std::unique_ptr<field> &&field)
    {
        if (!fields.emplace(field->get_name(), std::move(field)).second)
            throw std::runtime_error("Field already exists.");
    }
} // namespace riddle