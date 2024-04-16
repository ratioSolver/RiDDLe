#include "item.hpp"
#include "type.hpp"
#include "core.hpp"

namespace riddle
{
    item::item(type &t) : tp(t) {}

    [[nodiscard]] std::shared_ptr<bool_item> operator==(std::shared_ptr<item> lhs, std::shared_ptr<item> rhs) noexcept { return lhs->get_type().get_scope().get_core().eq(lhs, rhs); }
    [[nodiscard]] std::shared_ptr<bool_item> operator!=(std::shared_ptr<item> lhs, std::shared_ptr<item> rhs) noexcept { return !(lhs == rhs); }

    bool_item::bool_item(bool_type &t, const utils::lit &l) : item(t), value(l) {}

    [[nodiscard]] std::shared_ptr<bool_item> operator&&(std::shared_ptr<bool_item> lhs, std::shared_ptr<bool_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().conj({lhs, rhs}); }
    [[nodiscard]] std::shared_ptr<bool_item> operator||(std::shared_ptr<bool_item> lhs, std::shared_ptr<bool_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().disj({lhs, rhs}); }
    [[nodiscard]] std::shared_ptr<bool_item> operator!(std::shared_ptr<bool_item> lhs) noexcept { return lhs->get_type().get_scope().get_core().negate(lhs); }

    arith_item::arith_item(int_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(real_type &t, const utils::lin &l) : item(t), value(l) {}
    arith_item::arith_item(time_type &t, const utils::lin &l) : item(t), value(l) {}

    [[nodiscard]] std::shared_ptr<arith_item> operator+(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().add({lhs, rhs}); }
    [[nodiscard]] std::shared_ptr<arith_item> operator-(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().sub({lhs, rhs}); }
    [[nodiscard]] std::shared_ptr<arith_item> operator*(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().mul({lhs, rhs}); }
    [[nodiscard]] std::shared_ptr<arith_item> operator/(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().div({lhs, rhs}); }

    [[nodiscard]] std::shared_ptr<bool_item> operator<(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().lt(lhs, rhs); }
    [[nodiscard]] std::shared_ptr<bool_item> operator<=(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().leq(lhs, rhs); }
    [[nodiscard]] std::shared_ptr<bool_item> operator>(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().gt(lhs, rhs); }
    [[nodiscard]] std::shared_ptr<bool_item> operator>=(std::shared_ptr<arith_item> lhs, std::shared_ptr<arith_item> rhs) noexcept { return lhs->get_type().get_scope().get_core().geq(lhs, rhs); }

    string_item::string_item(string_type &t, const std::string &s) : item(t), value(s) {}

    enum_item::enum_item(type &t, VARIABLE_TYPE v) : item(t), env(t.get_scope().get_core()), value(v) {}
    std::shared_ptr<item> enum_item::get(const std::string &name) { return get_core().get(*this, name); }

    component::component(component_type &t, std::shared_ptr<env> parent) : item(t), env(t.get_scope().get_core(), parent) {}

    atom::atom(predicate &t, bool is_fact, const utils::lit &sigma, std::map<std::string, std::shared_ptr<item>> &&args) : item(t), env(t.get_scope().get_core(), nullptr, std::move(args)), fact(is_fact), sigma(sigma) {}
} // namespace riddle