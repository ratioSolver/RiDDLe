#include "core.h"
#include "item.h"
#include <stdexcept>

namespace riddle
{
    RIDDLE_EXPORT core::core() : scope(*this), env(*this) {}

    RIDDLE_EXPORT expr core::new_bool() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_bool([[maybe_unused]] bool value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT expr core::new_int() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_int([[maybe_unused]] utils::I value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT expr core::new_real() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_real([[maybe_unused]] utils::rational value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT expr core::get(const std::string &name) const
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }
} // namespace riddle