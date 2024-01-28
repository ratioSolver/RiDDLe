#include "item.hpp"
#include "type.hpp"

namespace riddle
{
    component::component(type &t, std::shared_ptr<env> parent) : item(t), env(t.get_core(), parent) {}
} // namespace riddle