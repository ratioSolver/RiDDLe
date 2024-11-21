#include "scope.hpp"
#include <stdexcept>

namespace riddle
{
    field::field(type &tp, std::string &&name) noexcept : tp(tp), name(std::move(name)) {}

    scope::scope(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args) : cr(c), parent(parent)
    {
        for (auto &arg : args)
            add_field(std::move(arg));
    }

    void scope::add_field(std::unique_ptr<field> field)
    {
        if (!fields.emplace(field->get_name(), std::move(field)).second)
            throw std::runtime_error("Field already exists.");
    }
} // namespace riddle
