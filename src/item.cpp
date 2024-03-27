#include "item.hpp"
#include "type.hpp"

namespace riddle
{
    component::component(type &t, std::shared_ptr<env> parent) : item(t), env(t.get_scope().get_core(), parent) {}

    atom::atom(predicate &t, bool is_fact, std::map<std::string, std::shared_ptr<item>> &&args) : item(t), env(t.get_scope().get_core(), nullptr, std::move(args)), fact(is_fact) {}
} // namespace riddle