#include "core.h"
#include <stdexcept>

namespace riddle
{
    RIDDLE_EXPORT core::core() : scope(*this), env(*this, this)
    {
        types.emplace("bool", new bool_type(*this));
        types.emplace("int", new int_type(*this));
        types.emplace("real", new real_type(*this));
    }

    RIDDLE_EXPORT expr core::new_bool() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_bool([[maybe_unused]] bool value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT expr core::new_int() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_int([[maybe_unused]] utils::I value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT expr core::new_real() { throw std::logic_error("not implemented"); }
    RIDDLE_EXPORT expr core::new_real([[maybe_unused]] utils::rational value) { throw std::logic_error("not implemented"); }

    RIDDLE_EXPORT type &core::get_type(const std::string &name)
    {
        auto it = types.find(name);
        if (it != types.end())
            return *it->second;
        throw std::out_of_range("type `" + name + "` not found");
    }

    RIDDLE_EXPORT method &core::get_method(const std::string &name, const std::vector<std::reference_wrapper<type>> &args)
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
        throw std::out_of_range("method `" + name + "` not found");
    }

    RIDDLE_EXPORT expr &core::get(const std::string &name)
    {
        auto it = items.find(name);
        if (it != items.end())
            return it->second;
        throw std::out_of_range("item `" + name + "` not found");
    }
} // namespace riddle