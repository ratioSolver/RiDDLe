#include "core.h"
#include "item.h"
#include <stdexcept>

namespace riddle
{
    RIDDLE_EXPORT core::core() : scope(*this), bt(new bool_type(*this)), it(new int_type(*this)), rt(new real_type(*this)), tt(new time_point_type(*this)), st(new string_type(*this))
    {
        types.emplace(bt->get_name(), bt);
        types.emplace(it->get_name(), it);
        types.emplace(rt->get_name(), rt);
        types.emplace(tt->get_name(), tt);
        types.emplace(st->get_name(), st);
    }

    RIDDLE_EXPORT type &core::get_type(const std::string &tp_name)
    {
        auto it = types.find(tp_name);
        if (it != types.end())
            return *it->second;
        throw std::out_of_range("type `" + tp_name + "` not found");
    }

    RIDDLE_EXPORT method &core::get_method(const std::string &m_name, const std::vector<std::reference_wrapper<type>> &args)
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
                        if (!m_args[i].get().get_type().is_assignable_from(args[i].get()))
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
            }
        throw std::out_of_range("method `" + m_name + "` not found");
    }

    RIDDLE_EXPORT predicate &core::get_predicate(const std::string &name)
    {
        auto it = predicates.find(name);
        if (it != predicates.end())
            return *it->second;
        throw std::out_of_range("predicate `" + name + "` not found");
    }

    RIDDLE_EXPORT expr &core::get(const std::string &name)
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }
} // namespace riddle