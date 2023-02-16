#include "type.h"
#include "core.h"
#include "item.h"

namespace riddle
{
    RIDDLE_EXPORT type::type(core &cr, const std::string &name) : cr(cr), name(name) {}

    bool_type::bool_type(core &cr) : type(cr, "bool") {}
    expr bool_type::new_instance() { return cr.new_bool(); }

    int_type::int_type(core &cr) : type(cr, "int") {}
    expr int_type::new_instance() { return cr.new_int(); }

    real_type::real_type(core &cr) : type(cr, "real") {}
    expr real_type::new_instance() { return cr.new_real(); }

    RIDDLE_EXPORT complex_type::complex_type(scope &scp, const std::string &name) : scope(scp), type(scp.get_core(), name) {}

    RIDDLE_EXPORT type &complex_type::get_type(const std::string &name)
    {
        auto it = types.find(name);
        if (it != types.end())
            return *it->second;
        return scope::get_type(name);
    }

    RIDDLE_EXPORT method &complex_type::get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args)
    {
        auto it = methods.find(name);
        if (it != methods.end())
            for (auto &m : it->second)
            {
                auto &m_args = m->get_args();
                if (m_args.size() == args.size())
                {
                    bool match = true;
                    for (size_t i = 0; i < m_args.size(); ++i)
                        if (m_args[i].get().get_type() != args[i].get())
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
            }
        return scope::get_method(name, args);
    }

    RIDDLE_EXPORT expr complex_type::new_instance()
    {
        auto inst = new item(*this);
        return expr(inst);
    }
} // namespace riddle
