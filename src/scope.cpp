#include "scope.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT scope::scope(scope &scp) : scp(scp) {}

    RIDDLE_EXPORT field &scope::get_field(const std::string &name)
    {
        auto it = fields.find(name);
        if (it != fields.end())
            return *it->second;
        else
            return scp.get_field(name);
    }

    RIDDLE_EXPORT void scope::add_field(field_ptr f)
    {
        if (!fields.emplace(f->get_name(), std::move(f)).second)
            throw std::runtime_error("field already exists");
    }

    RIDDLE_EXPORT env::env(env_ptr parent) : parent(parent) {}

    RIDDLE_EXPORT expr &env::get(const std::string &name)
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        else
            return parent->get(name);
    }
} // namespace riddle