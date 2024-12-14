#include "core.hpp"
#include "item.hpp"
#include "type.hpp"
#include <stdexcept>

namespace riddle
{
    core::core() noexcept : scope(*this, *this), env(*this, *this) {}

    std::shared_ptr<bool_item> core::new_bool(const bool value) { return std::make_shared<bool_item>(static_cast<bool_type &>(get_type(bool_kw)), value ? utils::TRUE_lit : utils::FALSE_lit); }
    std::shared_ptr<arith_item> core::new_int(const INT_TYPE value) { return std::make_shared<arith_item>(static_cast<int_type &>(get_type(int_kw)), utils::lin(utils::rational(value))); }
    std::shared_ptr<arith_item> core::new_real(const utils::rational &value) { return std::make_shared<arith_item>(static_cast<real_type &>(get_type(real_kw)), utils::lin(value)); }

    field &core::get_field(std::string_view name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second;
        throw std::out_of_range("field " + std::string(name) + " not found");
    }

    type &core::get_type(std::string_view name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        throw std::out_of_range("type " + std::string(name) + " not found");
    }

    item &core::get(std::string_view name)
    {
        if (auto it = items.find(name); it != items.end())
            return *it->second;
        throw std::out_of_range("item " + std::string(name) + " not found");
    }

    void core::add_type(std::unique_ptr<type> t)
    {
        std::string name = t->get_name();
        if (!types.emplace(name, std::move(t)).second)
            throw std::invalid_argument("type " + name + " already exists");
    }
} // namespace riddle