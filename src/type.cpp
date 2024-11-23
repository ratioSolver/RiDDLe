#include "type.hpp"
#include "core.hpp"
#include "lexer.hpp"
#include <queue>
#include <stdexcept>

namespace riddle
{
    type::type(scope &scp, std::string &&name, bool primitive) noexcept : scp(scp), name(std::move(name)), primitive(primitive) {}

    bool_type::bool_type(core &cr) noexcept : type(cr, bool_kw, true) {}

    int_type::int_type(core &cr) noexcept : type(cr, int_kw, true) {}

    real_type::real_type(core &cr) noexcept : type(cr, real_kw, true) {}
    bool real_type::is_assignable_from(const type &other) const { return this == &other || &get_scope().get_core().get_type(int_kw) == &other; }

    time_type::time_type(core &cr) noexcept : type(cr, time_kw, true) {}
    bool time_type::is_assignable_from(const type &other) const { return this == &other || &get_scope().get_core().get_type(int_kw) == &other; }

    string_type::string_type(core &cr) noexcept : type(cr, string_kw, true) {}

    enum_type::enum_type(scope &scp, std::string &&name, std::vector<std::shared_ptr<item>> &&domain) noexcept : type(scp, std::move(name), false), domain(std::move(domain)) {}
    bool enum_type::is_assignable_from(const type &other) const
    {
        if (this == &other)
            return true;
        else if (auto et = dynamic_cast<const enum_type *>(&other))
        {
            std::queue<const enum_type *> q;
            q.push(et);
            while (!q.empty())
            {
                auto tp = q.front();
                q.pop();
                if (tp == this)
                    return true;
                for (const auto &e : tp->get_enums())
                    q.push(&e.get());
            }
        }
        return false;
    }

    component_type::component_type(scope &scp, std::string &&name) noexcept : scope(scp.get_core(), scp), type(scp, std::move(name), false) {}

    bool component_type::is_assignable_from(const type &other) const
    {
        if (this == &other)
            return true;
        else if (auto ct = dynamic_cast<const component_type *>(&other))
        {
            std::queue<const component_type *> q;
            q.push(ct);
            while (!q.empty())
            {
                auto tp = q.front();
                q.pop();
                if (tp == this)
                    return true;
                for (const auto &p : tp->parents)
                    q.push(&p.get());
            }
        }
        return false;
    }

    method &component_type::get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const
    {
        if (auto it = methods.find(name); it != methods.end())
            for (const auto &m : it->second)
                if (m->get_args().size() == argument_types.size())
                {
                    bool match = true;
                    for (size_t i = 0; i < argument_types.size(); ++i)
                        if (!m->get_field(m->get_args()[i]).get_type().is_assignable_from(argument_types[i].get()))
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
        try
        { // first check in any enclosing scope
            return get_scope().get_method(name, argument_types);
        }
        catch (const std::out_of_range &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &p : parents)
                try
                {
                    return p.get().get_method(name, argument_types);
                }
                catch (const std::out_of_range &)
                {
                }
            throw std::out_of_range("method " + std::string(name) + " not found");
        }
    }

    predicate::predicate(scope &scp, std::string &&name, std::vector<std::unique_ptr<field>> &&args) noexcept : scope(scp.get_core(), scp, std::move(args)), type(scp, std::move(name), false) {}
} // namespace riddle