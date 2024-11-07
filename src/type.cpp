#include <queue>
#include <cassert>
#include "type.hpp"
#include "core.hpp"
#include "declaration.hpp"

namespace riddle
{
    type::type(scope &scp, std::string_view name, bool primitive) : scp(scp), name(name), primitive(primitive) {}

    bool_type::bool_type(core &c) : type(c, "bool", true) {}
    std::shared_ptr<item> bool_type::new_instance() { return scp.get_core().new_bool(); }

    int_type::int_type(core &c) : type(c, "int", true) {}
    std::shared_ptr<item> int_type::new_instance() { return scp.get_core().new_int(); }

    real_type::real_type(core &c) : type(c, "real", true) {}
    bool real_type::is_assignable_from(const type &other) const { return this == &other || &other.get_scope().get_core().get_int_type() == &other || &other.get_scope().get_core().get_time_type() == &other; }
    std::shared_ptr<item> real_type::new_instance() { return scp.get_core().new_real(); }

    time_type::time_type(core &c) : type(c, "time", true) {}
    bool time_type::is_assignable_from(const type &other) const { return this == &other || &other.get_scope().get_core().get_int_type() == &other || &other.get_scope().get_core().get_real_type() == &other; }
    std::shared_ptr<item> time_type::new_instance() { return scp.get_core().new_time(); }

    string_type::string_type(core &c) : type(c, "string", true) {}
    std::shared_ptr<item> string_type::new_instance() { return scp.get_core().new_string(); }

    typedef_type::typedef_type(scope &parent, std::string_view name, type &base_type, expression &value) : type(parent, name), base_type(base_type), value(value) {}
    std::shared_ptr<item> typedef_type::new_instance()
    {
        // we create a new environment for the expression evaluation..
        auto ctx = std::make_shared<env>(scp.get_core());
        return value.evaluate(scp, ctx);
    }

    enum_type::enum_type(scope &parent, std::string_view name, std::vector<std::shared_ptr<item>> &&values) : type(parent, name), values(std::move(values)) {}
    std::vector<std::shared_ptr<item>> enum_type::get_values() const
    {
        std::vector<std::shared_ptr<item>> vals;
        for (const auto &val : values)
            vals.push_back(val);
        for (const auto &e : enums)
            for (const auto &val : e.get().get_values())
                vals.push_back(val);
        return vals;
    }
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
        std::vector<std::reference_wrapper<utils::enum_val>> enum_vals;
        for (const auto &value : get_values())
            enum_vals.push_back(*value);
        return scp.get_core().new_enum(*this, std::move(enum_vals));
    }

    component_type::component_type(scope &parent, std::string_view name) : type(parent, name), scope(parent.get_core(), parent) {}
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
                for (const auto &p : tp->get_parents())
                    q.push(&p.get());
            }
        }
        return false;
    }

    std::optional<std::reference_wrapper<constructor>> component_type::get_constructor(const std::vector<std::reference_wrapper<const type>> &argument_types) const
    {
        for (const auto &c : constructors)
            if (c->get_args().size() == argument_types.size())
            {
                bool match = true;
                for (size_t i = 0; i < argument_types.size(); ++i)
                    if (!c->get_args()[i].get().get_type().is_assignable_from(argument_types[i].get()))
                    {
                        match = false;
                        break;
                    }
                if (match)
                    return *c;
            }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<method>> component_type::get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const
    {
        if (auto it = methods.find(name); it != methods.end())
            for (const auto &m : it->second)
                if (m->get_arguments().size() == argument_types.size())
                {
                    bool match = true;
                    for (size_t i = 0; i < argument_types.size(); ++i)
                        if (!m->get_arguments()[i].get().get_type().is_assignable_from(argument_types[i].get()))
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
        // first check in any enclosing scope
        if (auto m = scp.get_core().get_method(name, argument_types))
            return m;
        // if not in any enclosing scope, check any superclass
        for (const auto &p : get_parents())
            if (auto m = p.get().get_method(name, argument_types))
                return m;
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<field>> component_type::get_field(std::string_view name) const noexcept
    {
        if (auto f = scope::get_field(name)) // first check in the current scope
            return f;
        // if not in the current scope, check any superclass
        for (const auto &p : get_parents())
            if (auto f = p.get().get_field(name))
                return f;
        return std::nullopt; // if not found in any scope
    }

    std::optional<std::reference_wrapper<type>> component_type::get_type(std::string_view name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        // first check in any enclosing scope
        if (auto t = scp.get_core().get_type(name))
            return t;
        // if not in any enclosing scope, check any superclass
        for (const auto &p : get_parents())
            if (auto t = p.get().get_type(name))
                return t;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<predicate>> component_type::get_predicate(std::string_view name) const
    {
        if (auto it = predicates.find(name); it != predicates.end())
            return *it->second;
        // first check in any enclosing scope
        if (auto p = scp.get_core().get_predicate(name))
            return p;
        // if not in any enclosing scope, check any superclass
        for (const auto &par : get_parents())
            if (auto p = par.get().get_predicate(name))
                return p;
        return std::nullopt;
    }

    std::shared_ptr<item> component_type::new_instance() { return std::make_shared<component>(*this); }

    void component_type::add_parent(component_type &parent) { parents.emplace_back(parent); }

    void component_type::add_parent(predicate &child, predicate &parent) { child.add_parent(parent); }

    void component_type::add_constructor(std::unique_ptr<constructor> &&ctor)
    {
        std::vector<std::reference_wrapper<const type>> args;
        for (const auto &arg : ctor->get_args())
            args.push_back(arg.get().get_type());
        if (get_constructor(args))
            throw std::runtime_error("constructor with the same arguments already exists");
        constructors.emplace_back(std::move(ctor));
    }
    void component_type::add_method(std::unique_ptr<method> &&meth)
    {
        std::vector<std::reference_wrapper<const type>> args;
        for (const auto &arg : meth->get_arguments())
            args.push_back(arg.get().get_type());
        if (get_method(meth->get_name(), args))
            throw std::runtime_error("method `" + meth->get_name() + "` with the same arguments already exists");
        methods[meth->get_name()].emplace_back(std::move(meth));
    }
    void component_type::add_type(std::unique_ptr<type> &&tp)
    {
        if (!types.emplace(tp->get_name(), std::move(tp)).second)
            throw std::runtime_error("type `" + tp->get_name() + "` already exists");
    }
    void component_type::add_predicate(std::unique_ptr<predicate> &&pred)
    {
        pred->add_field(std::make_unique<field>(*this, TAU_NAME)); // we add the tau field to the predicate..

        // we notify all the supertypes that a new predicate has been created..
        std::queue<component_type *> q;
        q.push(this);
        while (!q.empty())
        {
            auto &tp = *q.front();
            tp.new_predicate(*pred);
            for (const auto &p : tp.get_parents())
                q.push(&p.get());
            q.pop();
        }

        // we add the predicate to the current scope..
        if (!predicates.emplace(pred->get_name(), std::move(pred)).second)
            throw std::runtime_error("predicate `" + pred->get_name() + "` already exists");
    }

    predicate::predicate(scope &parent, std::string_view name, std::vector<std::unique_ptr<field>> &&args, const std::vector<std::unique_ptr<statement>> &body) : type(parent, name), scope(parent.get_core(), parent, std::move(args)), body(body) {}
    bool predicate::is_assignable_from(const type &other) const
    {
        if (this == &other)
            return true;
        else if (auto pt = dynamic_cast<const predicate *>(&other))
        {
            std::queue<const predicate *> q;
            q.push(pt);
            while (!q.empty())
            {
                auto tp = q.front();
                q.pop();
                if (tp == this)
                    return true;
                for (const auto &p : tp->get_parents())
                    q.push(&p.get());
            }
        }
        return false;
    }
    void predicate::call(std::shared_ptr<atom> atm)
    {
        assert(is_assignable_from(atm->get_type()));
        for (auto &p : parents)
            p.get().call(atm);
        auto ctx = std::make_shared<env>(scp.get_core(), atm);
        for (const auto &stmt : body)
            stmt->execute(*this, ctx);
    }

    void predicate::add_parent(predicate &parent) { parents.emplace_back(parent); }

    std::shared_ptr<item> predicate::new_instance() { return scp.get_core().new_fact(*this); }

    std::optional<std::reference_wrapper<field>> predicate::get_field(std::string_view name) const noexcept
    {
        if (auto f = scope::get_field(name)) // first check in the current scope
            return f;
        // if not in the current scope, check any superclass
        for (const auto &p : get_parents())
            if (auto f = p.get().get_field(name))
                return f;
        return std::nullopt; // if not found in any scope
    }
} // namespace riddle