#include <algorithm>
#include "core.hpp"

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

    bool is_bool(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_bool_type() == &x.get_type(); }
    bool is_int(const riddle::item &x) noexcept
    {
        if (&x.get_type().get_scope().get_core().get_int_type() == &x.get_type())
            return true;
        if (x.get_type().get_scope().get_core().is_constant(x))
        {
            auto val = x.get_type().get_scope().get_core().arithmetic_value(x);
            return val.get_infinitesimal() == 0 && val.get_rational().denominator() == 1;
        }
        return false;
    }
    bool is_real(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_real_type() == &x.get_type() || x.get_type().get_scope().get_core().is_constant(x); }
    bool is_time(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_time_type() == &x.get_type() || x.get_type().get_scope().get_core().is_constant(x); }

    type &determine_type(const std::vector<std::shared_ptr<item>> &xprs)
    {
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_int(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_int_type();
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_int(*x) || is_real(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_real_type();
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_time(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_time_type();
        return xprs[0]->get_type();
    }
} // namespace riddle