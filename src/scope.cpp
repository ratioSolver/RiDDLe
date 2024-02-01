#include "scope.hpp"

namespace riddle
{
    scope::scope(core &c, std::shared_ptr<scope> parent) : c(c), parent(parent) {}

    std::optional<std::reference_wrapper<field>> scope::get_field(const std::string &name) const noexcept
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second.get();
        else if (parent != nullptr)
            return parent->get_field(name);
        else
            return std::nullopt;
    }

    void scope::add_field(std::unique_ptr<field> &&field)
    {
        if (!fields.emplace(field->get_name(), std::move(field)).second)
            throw std::runtime_error("Field already exists.");
    }
} // namespace riddle