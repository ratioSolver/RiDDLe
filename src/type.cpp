#include "type.h"
#include "core.h"
#include "item.h"

namespace riddle
{
    type::type(core &cr, const std::string &name) : cr(cr), name(name) {}

    bool_type::bool_type(core &cr) : type(cr, BOOL_KW) {}
    expr bool_type::new_instance() { return cr.new_bool(); }

    int_type::int_type(core &cr) : type(cr, INT_KW) {}
    expr int_type::new_instance() { return cr.new_int(); }

    real_type::real_type(core &cr) : type(cr, REAL_KW) {}
    expr real_type::new_instance() { return cr.new_real(); }

    string_type::string_type(core &cr) : type(cr, STRING_KW) {}
    expr string_type::new_instance() { return cr.new_string(); }

    RIDDLE_EXPORT predicate::predicate(scope &scp, const std::string &name) : scope(scp), type(scp.get_core(), name) {}

    expr predicate::new_instance() { throw std::runtime_error("Predicate cannot be instantiated"); }
    expr predicate::new_fact() { return cr.new_fact(*this); }
    expr predicate::new_goal() { return cr.new_goal(*this); }

    RIDDLE_EXPORT complex_type::complex_type(scope &scp, const std::string &name) : scope(scp), type(scp.get_core(), name) {}

    RIDDLE_EXPORT type &complex_type::get_type(const std::string &tp_name)
    {
        auto it = types.find(tp_name);
        if (it != types.end())
            return *it->second;
        return scope::get_type(tp_name);
    }

    RIDDLE_EXPORT method &complex_type::get_method(const std::string &m_name, const std::vector<std::reference_wrapper<type>> &args)
    {
        auto it = methods.find(m_name);
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
        return scope::get_method(m_name, args);
    }

    expr complex_type::new_instance()
    {
        auto inst = new complex_item(*this);
        instances.push_back(inst);
        return inst;
    }
} // namespace riddle
