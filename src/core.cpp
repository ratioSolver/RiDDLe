#include "core.hpp"
#include "type.hpp"

namespace riddle
{
    core::core() : scope(*this), env(*this), bool_tp(*types.emplace("bool", std::make_unique<bool_type>(*this)).first->second), int_tp(*types.emplace("int", std::make_unique<int_type>(*this)).first->second), real_tp(*types.emplace("real", std::make_unique<real_type>(*this)).first->second), time_tp(*types.emplace("time", std::make_unique<time_type>(*this)).first->second), string_tp(*types.emplace("string", std::make_unique<string_type>(*this)).first->second) {}

    std::shared_ptr<item> core::new_item(component_type &tp) { return std::make_shared<component>(tp); }

    std::optional<std::reference_wrapper<field>> core::get_field(const std::string &name) const noexcept
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second.get();
        else
            return std::nullopt;
    }

    std::shared_ptr<item> core::get(const std::string &name) const noexcept
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else
            return nullptr;
    }
} // namespace riddle