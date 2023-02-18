#include "type.h"
#include "core.h"
#include "item.h"
#include "constructor.h"
#include <queue>

namespace riddle
{
    type::type(scope &scp, const std::string &name, bool primitive) : scp(scp), name(name), primitive(primitive) {}

    bool type::is_assignable_from(const type &other) const
    {
        std::queue<const type *> q;
        q.push(&other);
        while (!q.empty())
        {
            if (q.front() == this)
                return true;
            else if (auto ct = dynamic_cast<const complex_type *>(q.front()))
            {
                for (const auto &st : ct->get_parents())
                    q.push(&st.get());
                q.pop();
            }
            else
                return false;
        }
        return false;
    }

    bool_type::bool_type(core &cr) : type(cr, BOOL_KW) {}
    expr bool_type::new_instance() { return scp.get_core().new_bool(); }

    int_type::int_type(core &cr) : type(cr, INT_KW) {}
    expr int_type::new_instance() { return scp.get_core().new_int(); }

    real_type::real_type(core &cr) : type(cr, REAL_KW) {}
    expr real_type::new_instance() { return scp.get_core().new_real(); }

    string_type::string_type(core &cr) : type(cr, STRING_KW) {}
    expr string_type::new_instance() { return scp.get_core().new_string(); }

    time_point_type::time_point_type(core &cr) : type(cr, TIME_POINT_KW) {}
    expr time_point_type::new_instance() { return scp.get_core().new_time_point(); }

    typedef_type::typedef_type(scope &scp, const std::string &name, type &tp, const ast::expression_ptr &xpr) : type(scp, name), tp(tp), expr(xpr) {}
    expr typedef_type::new_instance()
    {
        env ctx(scp.get_core());
        return expr->evaluate(scp, ctx);
    }

    RIDDLE_EXPORT predicate::predicate(scope &scp, const std::string &name, std::vector<field_ptr> as, const std::vector<ast::statement_ptr> &body) : scope(scp), type(scp.get_core(), name), body(body)
    {
        if (!is_core(scp))
            add_field(new field(static_cast<complex_type &>(scp), THIS_KW, nullptr, true));
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    expr predicate::new_fact()
    {
        auto fact = get_core().new_fact(*this);
        instances.emplace_back(fact);
        return fact;
    }
    expr predicate::new_goal()
    {
        auto goal = get_core().new_goal(*this);
        instances.emplace_back(goal);
        return goal;
    }

    RIDDLE_EXPORT void predicate::call(expr &atm)
    {
        env ctx(static_cast<complex_item *>(atm.operator->()));
        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);
    }

    RIDDLE_EXPORT complex_type::complex_type(scope &scp, const std::string &name) : scope(scp), type(scp.get_core(), name) {}

    RIDDLE_EXPORT constructor &complex_type::get_constructor(const std::vector<std::reference_wrapper<type>> &args)
    {
        for (auto &c : constructors)
        {
            auto &c_args = c->get_args();
            if (c_args.size() == args.size())
            {
                bool match = true;
                for (size_t i = 0; i < c_args.size(); ++i)
                    if (!c_args[i].get().get_type().is_assignable_from(args[i].get()))
                    {
                        match = false;
                        break;
                    }
                if (match)
                    return *c;
            }
        }
        throw std::out_of_range("constructor not found");
    }

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
                        if (!m_args[i].get().get_type().is_assignable_from(args[i].get()))
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

    RIDDLE_EXPORT predicate &complex_type::get_predicate(const std::string &p_name)
    {
        auto it = predicates.find(p_name);
        if (it != predicates.end())
            return *it->second;
        return scope::get_predicate(p_name);
    }

    expr complex_type::new_instance()
    {
        auto inst = new complex_item(*this);
        instances.push_back(inst);
        return inst;
    }
} // namespace riddle
