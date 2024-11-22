#include "type.hpp"
#include "core.hpp"
#include "lexer.hpp"

namespace riddle
{
    type::type(scope &scp, std::string &&name, bool primitive) noexcept : scp(scp), name(std::move(name)), primitive(primitive) {}

    bool_type::bool_type(core &cr) noexcept : type(cr, bool_kw, true) {}
} // namespace riddle