#include "core.h"
#include "item.h"
#include <fstream>
#include <stdexcept>

#ifdef BUILD_LISTENERS
#include "core_listener.h"
#endif
#ifdef COMPUTE_NAMES
#include <queue>
#endif

namespace riddle
{
    RIDDLE_EXPORT core::core() : scope(*this), bt(new bool_type(*this)), it(new int_type(*this)), rt(new real_type(*this)), tt(new time_point_type(*this)), st(new string_type(*this))
    { // we add the basic types to the core..
        types.emplace(bt->get_name(), bt);
        types.emplace(it->get_name(), it);
        types.emplace(rt->get_name(), rt);
        types.emplace(tt->get_name(), tt);
        types.emplace(st->get_name(), st);
    }

    RIDDLE_EXPORT void core::read(const std::string &script)
    {
        parser prs(script);
        auto cu = prs.parse();
        cu->declare(*this);
        cu->refine(*this);
        cu->refine_predicates(*this);
        cu->execute(*this, *this);
        cus.emplace_back(std::move(cu));
        RECOMPUTE_NAMES();
        FIRE_READ(script);
    }

    RIDDLE_EXPORT void core::read(const std::vector<std::string> &files)
    {
        std::vector<ast::compilation_unit_ptr> c_cus;
        c_cus.reserve(files.size());

        for (auto &f : files)
            if (std::ifstream ifs(f); ifs)
            {
                parser prs(ifs);
                auto cu = prs.parse();
                c_cus.emplace_back(std::move(cu));
            }
            else
                throw std::invalid_argument("cannot find file `" + f + "`");

        for (auto &cu : c_cus)
            cu->declare(*this);
        for (auto &cu : c_cus)
            cu->refine(*this);
        for (auto &cu : c_cus)
            cu->refine_predicates(*this);
        for (auto &cu : c_cus)
            cu->execute(*this, *this);

        cus.reserve(cus.size() + c_cus.size());
        for (auto &cu : c_cus)
            cus.emplace_back(std::move(cu));

        RECOMPUTE_NAMES();
        FIRE_READ(files);
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

#ifdef BUILD_LISTENERS
    RIDDLE_EXPORT void core::fire_log(const std::string &msg) const noexcept
    {
        for (auto &l : listeners)
            l->log(msg);
    }
    RIDDLE_EXPORT void core::fire_read(const std::string &msg) const noexcept
    {
        for (auto &l : listeners)
            l->read(msg);
    }
    RIDDLE_EXPORT void core::fire_read(const std::vector<std::string> &files) const noexcept
    {
        for (auto &l : listeners)
            l->read(files);
    }
    RIDDLE_EXPORT void core::fire_state_changed() const noexcept
    {
        for (auto &l : listeners)
            l->state_changed();
    }
    RIDDLE_EXPORT void core::fire_started_solving() const noexcept
    {
        for (auto &l : listeners)
            l->started_solving();
    }
    RIDDLE_EXPORT void core::fire_solution_found() const noexcept
    {
        for (auto &l : listeners)
            l->solution_found();
    }
    RIDDLE_EXPORT void core::fire_inconsistent_problem() const noexcept
    {
        for (auto &l : listeners)
            l->inconsistent_problem();
    }
#endif

#ifdef COMPUTE_NAMES
    void core::recompute_names() noexcept
    {
        expr_names.clear();

        std::queue<std::pair<std::string, expr>> q;
        for (const auto &xpr : get_vars())
        {
            expr_names.emplace(&*xpr.second, xpr.first);
            if (!xpr.second->get_type().is_primitive())
                q.push(xpr);
        }

        while (!q.empty())
        {
            const auto &c_xpr = q.front();
            if (const auto *ci = dynamic_cast<complex_item *>(&*c_xpr.second))
                for (const auto &xpr : ci->get_vars())
                    if (expr_names.emplace(&*xpr.second, expr_names.at(&*c_xpr.second) + '.' + xpr.first).second)
                        if (!xpr.second->get_type().is_primitive())
                            q.push(xpr);
            q.pop();
        }
    }
#endif
} // namespace riddle