#include "type.h"
#include "core.h"
#include "item.h"
#include "constructor.h"
#include <queue>
#include <cassert>

namespace riddle
{
    type::type(scope &scp, const std::string &name, bool primitive) : scp(scp), name(name), primitive(primitive) {}

    RIDDLE_EXPORT bool type::is_assignable_from(const type &other) const
    {
        if (this == &other)
            return true;
        else if (auto ct = dynamic_cast<const complex_type *>(&other))
        { // we are comparing a complex type with another type..
            std::queue<const complex_type *> q;
            q.push(ct);
            while (!q.empty())
            {
                if (q.front() == this)
                    return true;
                for (const auto &st : q.front()->get_parents())
                    q.push(&st.get());
                q.pop();
            }
        }
        else if (auto pred = dynamic_cast<const predicate *>(&other))
        { // we are comparing a predicate with another type..
            std::queue<const predicate *> q;
            q.push(pred);
            while (!q.empty())
            {
                if (q.front() == this)
                    return true;
                for (const auto &st : q.front()->get_parents())
                    q.push(&st.get());
                q.pop();
            }
        }

        return false;
    }

    bool_type::bool_type(core &cr) : type(cr, BOOL_KW, true) {}
    expr bool_type::new_instance() { return scp.get_core().new_bool(); }

    int_type::int_type(core &cr) : type(cr, INT_KW, true) {}
    expr int_type::new_instance() { return scp.get_core().new_int(); }

    real_type::real_type(core &cr) : type(cr, REAL_KW, true) {}
    expr real_type::new_instance() { return scp.get_core().new_real(); }

    string_type::string_type(core &cr) : type(cr, STRING_KW, true) {}
    expr string_type::new_instance() { return scp.get_core().new_string(); }

    time_point_type::time_point_type(core &cr) : type(cr, TIME_POINT_KW, true) {}
    expr time_point_type::new_instance() { return scp.get_core().new_time_point(); }

    typedef_type::typedef_type(scope &scp, const std::string &name, type &tp, const ast::expression_ptr &xpr) : type(scp, name), tp(tp), xpr(xpr) {}
    expr typedef_type::new_instance()
    {
        // we create a new environment for the expression evaluation..
        env ctx(scp.get_core());
        return xpr->evaluate(scp, ctx);
    }

    enum_type::enum_type(scope &scp, const std::string &name) : type(scp, name) {}
    RIDDLE_EXPORT std::vector<expr> enum_type::get_all_values() const
    {
        std::vector<expr> values;
        values.reserve(instances.size());
        for (const auto &inst : instances)
            values.emplace_back(inst);
        for (const auto &e : enums)
        {
            auto ev = e.get().instances;
            values.reserve(values.size() + ev.size());
            for (const auto &v : ev)
                values.emplace_back(v);
        }
        return values;
    }
    expr enum_type::new_instance() { return scp.get_core().new_enum(*this, get_all_values()); }

    RIDDLE_EXPORT predicate::predicate(scope &scp, const std::string &name, std::vector<field_ptr> as, const std::vector<ast::statement_ptr> &body) : scope(scp), type(scp, name), body(body)
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

    RIDDLE_EXPORT field &predicate::get_field(const std::string &f_name) const
    {
        try
        { // first check in any enclosing scope
            return scope::get_field(f_name);
        }
        catch (const std::exception &)
        { // if not in any enclosing scope, check in the superclass
            for (const auto &tp : parents)
                try
                {
                    return tp.get().get_field(f_name);
                }
                catch (const std::exception &)
                {
                }
        }
        throw std::out_of_range("field `" + f_name + "` not found");
    }

    expr predicate::new_fact()
    { // we create a new fact..
        auto fact = type::get_core().new_fact(*this);
        instances.emplace_back(fact);
        return fact;
    }
    expr predicate::new_goal()
    { // we create a new goal..
        auto goal = type::get_core().new_goal(*this);
        instances.emplace_back(goal);
        return goal;
    }

    RIDDLE_EXPORT void predicate::call(expr &atm)
    { // we create a new environment for the rule's body execution..
        if (auto a = dynamic_cast<atom *>(atm.operator->()))
        {
            for (const auto &sp : parents)
                sp.get().call(atm);

            env ctx(a);
            for (const auto &stmnt : body)
                stmnt->execute(*this, ctx);
        }
        else
            throw std::runtime_error("invalid atom type");
    }

    RIDDLE_EXPORT complex_type::complex_type(scope &scp, const std::string &name) : scope(scp), type(scp.get_core(), name)
    { // we add the "this" field..
        add_field(new field(*this, THIS_KW, nullptr, true));
    }

    RIDDLE_EXPORT field &complex_type::get_field(const std::string &f_name) const
    {
        try
        { // first check in any enclosing scope
            return scope::get_field(f_name);
        }
        catch (const std::exception &)
        { // if not in any enclosing scope, check in the superclass
            for (const auto &tp : parents)
                try
                {
                    return tp.get().get_field(f_name);
                }
                catch (const std::exception &)
                {
                }
        }
        throw std::out_of_range("field `" + f_name + "` not found");
    }

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

    RIDDLE_EXPORT std::vector<std::reference_wrapper<constructor>> complex_type::get_constructors() const
    {
        std::vector<std::reference_wrapper<constructor>> ctors;
        for (const auto &c : constructors)
            ctors.emplace_back(*c);
        return ctors;
    }

    RIDDLE_EXPORT type &complex_type::get_type(const std::string &tp_name) const
    {
        auto it = types.find(tp_name);
        if (it != types.end())
            return *it->second;
        try
        { // first check in any enclosing scope
            return scope::get_type(tp_name);
        }
        catch (const std::exception &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &st : parents)
                try
                {
                    return st.get().get_type(tp_name);
                }
                catch (const std::out_of_range &)
                {
                }
        }
        throw std::out_of_range("type `" + tp_name + "` not found");
    }

    RIDDLE_EXPORT std::vector<std::reference_wrapper<type>> complex_type::get_types() const
    {
        std::vector<std::reference_wrapper<type>> tps;
        for (const auto &tp : types)
            tps.emplace_back(*tp.second);
        return tps;
    }

    RIDDLE_EXPORT method &complex_type::get_method(const std::string &m_name, const std::vector<std::reference_wrapper<type>> &args) const
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

        try
        { // first check in any enclosing scope
            return scope::get_method(m_name, args);
        }
        catch (const std::exception &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &st : parents)
                try
                {
                    return st.get().get_method(m_name, args);
                }
                catch (const std::out_of_range &)
                {
                }
        }
        throw std::out_of_range("method `" + m_name + "` not found");
    }

    RIDDLE_EXPORT std::vector<std::reference_wrapper<method>> complex_type::get_methods() const
    {
        std::vector<std::reference_wrapper<method>> mths;
        for (const auto &mth : methods)
            for (const auto &m : mth.second)
                mths.emplace_back(*m);
        return mths;
    }

    RIDDLE_EXPORT predicate &complex_type::get_predicate(const std::string &p_name) const
    {
        auto it = predicates.find(p_name);
        if (it != predicates.end())
            return *it->second;
        try
        { // first check in any enclosing scope
            return scope::get_predicate(p_name);
        }
        catch (const std::exception &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &st : parents)
                try
                {
                    return st.get().get_predicate(p_name);
                }
                catch (const std::out_of_range &)
                {
                }
        }
        throw std::out_of_range("predicate `" + p_name + "` not found");
    }

    RIDDLE_EXPORT std::vector<std::reference_wrapper<predicate>> complex_type::get_predicates() const
    {
        std::vector<std::reference_wrapper<predicate>> preds;
        for (const auto &pred : predicates)
            preds.emplace_back(*pred.second);
        return preds;
    }

    RIDDLE_EXPORT expr complex_type::new_instance()
    {
        auto inst = type::get_core().new_item(*this);
        if (parents.empty())
        { // if this is a root type, we store the instance in this type..
            instances.push_back(inst);
            return inst;
        } // otherwise, we store the instance in this type and in all the supertypes..
        std::queue<complex_type *> q;
        q.push(this);
        while (!q.empty())
        {
            auto &tp = *q.front();
            tp.instances.push_back(inst);
            for (auto &parent : tp.parents)
                q.push(&parent.get());
            q.pop();
        }
        return inst;
    }

    RIDDLE_EXPORT void complex_type::add_predicate(predicate_ptr &&pred)
    {
        // we add the tau field to the predicate..
        pred->add_field(new field(*this, TAU_KW));
        // we notify all the supertypes that a new predicate has been created..
        std::queue<complex_type *> q;
        q.push(this);
        while (!q.empty())
        {
            auto &tp = *q.front();
            tp.new_predicate(*pred);
            for (auto &parent : tp.parents)
                q.push(&parent.get());
            q.pop();
        }
        predicates.emplace(pred->get_name(), std::move(pred));
    }
} // namespace riddle
