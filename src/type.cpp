#include "type.hpp"
#include "core.hpp"
#include "lexer.hpp"
#include "exceptions.hpp"
#include <queue>
#include <cassert>

namespace riddle
{
    type::type(scope &scp, std::string &&name, bool primitive) noexcept : scp(scp), name(std::move(name)), primitive(primitive) {}
    std::string type::get_full_name() const noexcept
    {
        std::string full_name = get_name();
        const auto *t = this;
        while (const auto *et = dynamic_cast<const type *>(&t->get_scope()))
        {
            full_name.insert(0, et->get_name() + ".");
            t = et;
        }
        return full_name;
    }

    bool_type::bool_type(core &cr) noexcept : type(cr, bool_kw, true) {}
    std::shared_ptr<item> bool_type::new_instance() { return get_scope().get_core().new_bool(); }

    int_type::int_type(core &cr) noexcept : type(cr, int_kw, true) {}
    bool int_type::is_assignable_from(const type &other) const { return this == &other || &get_scope().get_core().get_type(real_kw) == &other || &get_scope().get_core().get_type(time_kw) == &other; }
    std::shared_ptr<item> int_type::new_instance() { return get_scope().get_core().new_int(); }

    real_type::real_type(core &cr) noexcept : type(cr, real_kw, true) {}
    bool real_type::is_assignable_from(const type &other) const { return this == &other || &get_scope().get_core().get_type(int_kw) == &other || &get_scope().get_core().get_type(time_kw) == &other; }
    std::shared_ptr<item> real_type::new_instance() { return get_scope().get_core().new_real(); }

    time_type::time_type(core &cr) noexcept : type(cr, time_kw, true) {}
    bool time_type::is_assignable_from(const type &other) const { return this == &other || &get_scope().get_core().get_type(int_kw) == &other || &get_scope().get_core().get_type(real_kw) == &other; }
    std::shared_ptr<item> time_type::new_instance() { return get_scope().get_core().new_time(); }

    string_type::string_type(core &cr) noexcept : type(cr, string_kw, true) {}
    std::shared_ptr<item> string_type::new_instance() { return get_scope().get_core().new_string(); }

    enum_type::enum_type(scope &scp, std::string &&name, std::vector<std::shared_ptr<item>> &&domain) noexcept : type(scp, std::move(name), true), domain(std::move(domain)) {}
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
    std::shared_ptr<item> enum_type::new_instance()
    {
        switch (domain.size())
        {
        case 0:
            throw inconsistency_exception();
        case 1:
            return domain[0];
        default:
        {
            std::vector<std::reference_wrapper<utils::enum_val>> items;
            for (const auto &i : domain)
                items.push_back(*i);
            return get_scope().get_core().new_enum(*this, std::move(items));
        }
        }
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

    constructor &component_type::get_constructor(const std::vector<std::reference_wrapper<const type>> &argument_types) const
    {
        for (const auto &c : constructors)
            if (c->get_args().size() == argument_types.size())
            {
                bool match = true;
                for (size_t i = 0; i < argument_types.size(); ++i)
                    if (!c->get_field(c->get_args()[i]).get_type().is_assignable_from(argument_types[i].get()))
                    {
                        match = false;
                        break;
                    }
                if (match)
                    return *c;
            }
        throw std::out_of_range("constructor not found");
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
    type &component_type::get_type(std::string_view name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        try
        { // first check in any enclosing scope
            return get_scope().get_type(name);
        }
        catch (const std::out_of_range &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &p : parents)
                try
                {
                    return p.get().get_type(name);
                }
                catch (const std::out_of_range &)
                {
                }
            throw std::out_of_range("type " + std::string(name) + " not found");
        }
    }
    predicate &component_type::get_predicate(std::string_view name) const
    {
        if (auto it = predicates.find(name); it != predicates.end())
            return *it->second;
        try
        { // first check in any enclosing scope
            return get_scope().get_predicate(name);
        }
        catch (const std::out_of_range &)
        { // if not in any enclosing scope, check any superclass
            for (const auto &p : parents)
                try
                {
                    return p.get().get_predicate(name);
                }
                catch (const std::out_of_range &)
                {
                }
            throw std::out_of_range("predicate " + std::string(name) + " not found");
        }
    }

    void component_type::add_parent(component_type &parent) { parents.emplace_back(parent); }

    void component_type::add_constructor(std::unique_ptr<constructor> ctr)
    {
        std::vector<std::reference_wrapper<const type>> args;
        args.reserve(ctr->get_args().size());
        for (const auto &arg : ctr->get_args())
            args.push_back(get_field(arg).get_type());
        for (const auto &c : constructors)
            if (c->get_args().size() == args.size())
            {
                bool match = true;
                for (size_t i = 0; i < args.size(); ++i)
                    if (!c->get_field(c->get_args()[i]).get_type().is_assignable_from(args[i]))
                    {
                        match = false;
                        break;
                    }
                if (match)
                    throw std::invalid_argument("constructor already exists");
            }
        constructors.emplace_back(std::move(ctr));
    }

    void component_type::add_method(std::unique_ptr<method> mthd)
    {
        std::vector<std::reference_wrapper<const type>> args;
        for (const auto &arg : mthd->get_args())
            args.push_back(mthd->get_field(arg).get_type());
        try
        { // check if the method already exists
            [[maybe_unused]] auto &m = get_method(mthd->get_name(), args);
            throw std::invalid_argument("method `" + mthd->get_name() + "` already exists");
        }
        catch (const std::out_of_range &)
        {
            methods[mthd->get_name()].push_back(std::move(mthd));
        }
    }

    void component_type::add_predicate(std::unique_ptr<predicate> pred)
    {
        std::string name = pred->get_name();
        if (!predicates.emplace(name, std::move(pred)).second)
            throw std::invalid_argument("predicate " + name + " already exists");
    }

    void component_type::add_type(std::unique_ptr<type> t)
    {
        std::string name = t->get_name();
        if (!types.emplace(name, std::move(t)).second)
            throw std::invalid_argument("type " + name + " already exists");
    }

    std::shared_ptr<item> component_type::new_instance()
    {
        auto itm = std::make_shared<component>(static_cast<component_type &>(*this));
        // we store the instance in type the hierarchy..
        std::queue<component_type *> q;
        q.push(this);
        while (!q.empty())
        {
            auto tp = q.front();
            q.pop();
            tp->instances.push_back(itm);
            for (const auto &p : tp->parents)
                q.push(&p.get());
        }
        return itm;
    }

    predicate::predicate(scope &scp, std::string &&name, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body) noexcept : scope(scp.get_core(), scp, std::move(args)), type(scp, std::move(name), false), body(body) {}

    void predicate::call(atom_expr atm)
    {
        assert(is_assignable_from(atm->get_type()));
        for (auto &p : parents)
            p.get().call(atm);
        env ctx(get_core(), *atm);
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);
    }

    std::shared_ptr<item> predicate::new_instance()
    {
        auto atm = get_scope().get_core().new_atom(true, *this);
        // we store the atom in the predicate hierarchy..
        std::queue<predicate *> q;
        q.push(this);
        while (!q.empty())
        {
            auto p = q.front();
            q.pop();
            p->atoms.push_back(atm);
            for (const auto &par : p->parents)
                q.push(&par.get());
        }
        return atm;
    }
} // namespace riddle