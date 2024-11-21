#include "type.hpp"

namespace riddle
{
    type::type(std::string &&name, bool primitive) noexcept : name(std::move(name)), primitive(primitive) {}
} // namespace riddle