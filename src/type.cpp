#include "type.hpp"

namespace riddle
{
    type::type(std::shared_ptr<scope> parent, const std::string &name) : scope(parent->get_core(), parent), name(name) {}
} // namespace riddle