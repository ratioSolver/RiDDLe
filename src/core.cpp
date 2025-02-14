#include "core.hpp"
#include <sstream>
#include <fstream>
#include <queue>
#include <set>
#include <algorithm>
#include <cassert>

namespace riddle
{
    core::core(std::string_view name) noexcept : scope(*this, *this), env(*this, *this), name(name)
    {
        add_type(utils::make_u_ptr<bool_type>(*this));
        add_type(utils::make_u_ptr<int_type>(*this));
        add_type(utils::make_u_ptr<real_type>(*this));
        add_type(utils::make_u_ptr<time_type>(*this));
        add_type(utils::make_u_ptr<string_type>(*this));
    }

    void core::read(std::string &&script)
    {
        std::stringstream ss(script);
        parser p(ss);
        auto cu = p.parse_compilation_unit();
        cu->declare(*this);
        cu->refine(*this);
        cu->refine_predicates(*this);
        cu->execute(*this, *this);
        cus.push_back(std::move(cu)); // add the compilation unit to the list of compilation units
        RECOMPUTE_NAMES();
    }

    void core::read(const std::vector<std::string> &files)
    {
        std::vector<utils::u_ptr<compilation_unit>> c_cus;
        c_cus.reserve(files.size());
        for (const auto &file : files)
            if (std::ifstream ifs(file); ifs.is_open())
            {
                parser p(ifs);
                c_cus.push_back(p.parse_compilation_unit());
            }
            else
                throw std::runtime_error("file `" + file + "` not found");

        for (auto &cu : c_cus)
            cu->declare(*this);
        for (auto &cu : c_cus)
            cu->refine(*this);
        for (auto &cu : c_cus)
            cu->refine_predicates(*this);
        for (auto &cu : c_cus)
            cu->execute(*this, *this);

        cus.insert(cus.end(), std::make_move_iterator(c_cus.begin()), std::make_move_iterator(c_cus.end())); // add the compilation units to the list of compilation units
        RECOMPUTE_NAMES();
    }

    bool_expr core::new_and(std::vector<bool_expr> &&exprs)
    {
        assert(!exprs.empty());
        return utils::make_s_ptr<and_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_or(std::vector<bool_expr> &&exprs)
    {
        assert(!exprs.empty());
        return utils::make_s_ptr<or_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_xor(std::vector<bool_expr> &&exprs)
    {
        assert(!exprs.empty());
        return utils::make_s_ptr<xor_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_not(bool_expr expr) { return utils::make_s_ptr<bool_not>(static_cast<bool_type &>(get_type(bool_kw)), std::move(expr)); }

    atom_expr core::new_atom(bool is_fact, predicate &pred, std::map<std::string, expr, std::less<>> &&args)
    {
        auto atm = create_atom(is_fact, pred, std::move(args));
        std::queue<predicate *> q;
        q.push(&pred);
        while (!q.empty())
        {
            auto p = q.front();
            p->atoms.push_back(atm);
            q.pop();
            for (auto &parent : p->parents)
                q.push(&*parent);
        }
        return atm;
    }

    field &core::get_field(std::string_view name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second;
        throw std::out_of_range("field `" + std::string(name) + "` not found");
    }

    method &core::get_method(std::string_view name, const std::vector<utils::ref_wrapper<const type>> &argument_types) const
    {
        if (auto it = methods.find(name); it != methods.end())
            for (const auto &m : it->second)
                if (m->get_args().size() == argument_types.size())
                {
                    bool match = true;
                    for (size_t i = 0; i < argument_types.size(); ++i)
                        if (!m->get_field(m->get_args()[i]).get_type().is_assignable_from(*argument_types[i]))
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
        throw std::out_of_range("method `" + std::string(name) + "` not found");
    }
    type &core::get_type(std::string_view name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        throw std::out_of_range("type `" + std::string(name) + "` not found");
    }
    predicate &core::get_predicate(std::string_view name) const
    {
        if (auto it = predicates.find(name); it != predicates.end())
            return *it->second;
        throw std::out_of_range("predicate `" + std::string(name) + "` not found");
    }

    type &core::type_promotion(const std::vector<arith_expr> &exprs) const
    {
        assert(!exprs.empty());
        unsigned int max = 0;
        for (const auto &expr : exprs)
            if (expr->get_type().get_name() == int_kw)
                max = std::max(max, 1u);
            else if (expr->get_type().get_name() == real_kw)
                max = std::max(max, 2u);
            else if (expr->get_type().get_name() == time_kw)
            {
                max = 3u;
                break;
            }
        switch (max)
        {
        case 1:
            return get_type(int_kw);
        case 2:
            return get_type(real_kw);
        case 3:
            return get_type(time_kw);
        default:
            throw std::invalid_argument("invalid type promotion");
        }
    }

    expr core::get(std::string_view name)
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        throw std::out_of_range("item `" + std::string(name) + "` not found");
    }

    json::json core::to_json() const
    {
        std::set<term *> all_items;      // we keep track of all the items..
        std::set<atom_term *> all_atoms; // we keep track of all the atoms..
        std::queue<component_type *> q;
        for (const auto &tp : types)
            if (auto ct = dynamic_cast<component_type *>(tp.second.get()))
                q.push(ct);
            else if (auto et = dynamic_cast<enum_type *>(tp.second.get()))
                for (const auto &val : et->get_domain())
                    all_items.insert(static_cast<term *>(val.get()));
        while (!q.empty())
        {
            auto tp = q.front();
            q.pop();
            for (const auto &itm : tp->get_instances())
                all_items.insert(itm.get());
            for (const auto &pred : tp->get_predicates())
                for (const auto &atm : pred.second->get_atoms())
                    all_atoms.insert(static_cast<riddle::atom_term *>(atm.get()));
            for (const auto &etp : tp->get_types())
                if (auto ct = dynamic_cast<component_type *>(etp.second.get()))
                    q.push(ct);
                else if (auto et = dynamic_cast<enum_type *>(etp.second.get()))
                    for (const auto &val : et->get_domain())
                        all_items.insert(static_cast<term *>(val.get()));
        }
        for (const auto &pred : predicates)
            for (const auto &atm : pred.second->get_atoms())
                all_atoms.insert(static_cast<riddle::atom_term *>(atm.get()));

        json::json j_core{{"name", std::string_view(name)}};
        if (!all_items.empty())
        { // we add the items of the core..
            json::json j_items;
            for (const auto &itm : all_items)
                j_items[std::to_string(itm->get_id())] = itm->to_json();
            j_core["items"] = std::move(j_items);
        }
        if (!all_atoms.empty())
        { // we add the atoms of the core..
            json::json j_atoms;
            for (const auto &atm : all_atoms)
                j_atoms[std::to_string(atm->get_id())] = atm->to_json();
            j_core["atoms"] = std::move(j_atoms);
        }
        if (!items.empty()) // we add the fields of the core..
            j_core["exprs"] = env::to_json();

        return j_core;
    }

    void core::add_method(utils::u_ptr<method> mthd)
    {
        std::vector<utils::ref_wrapper<const type>> args;
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

    void core::add_predicate(utils::u_ptr<predicate> pred)
    {
        std::string name = pred->get_name();
        if (!predicates.emplace(name, std::move(pred)).second)
            throw std::invalid_argument("predicate `" + name + "` already exists");
    }

    void core::add_type(utils::u_ptr<type> t)
    {
        std::string name = t->get_name();
        if (!types.emplace(name, std::move(t)).second)
            throw std::invalid_argument("type `" + name + "` already exists");
    }

#ifdef COMPUTE_NAMES
    void core::recompute_names() noexcept
    {
        expr_names.clear();

        std::queue<std::pair<std::string, expr>> q;
        for (const auto &xpr : items)
        {
            expr_names.emplace(xpr.second.get(), xpr.first);
            if (!xpr.second->get_type().is_primitive())
                q.push(xpr);
        }

        while (!q.empty())
        {
            const auto &c_xpr = q.front();
            if (const auto *c_env = dynamic_cast<env *>(c_xpr.second.get()))
                for (const auto &xpr : c_env->items)
                    if (expr_names.emplace(xpr.second.get(), expr_names.at(c_xpr.second.get()) + '.' + xpr.first).second)
                        if (!xpr.second->get_type().is_primitive())
                            q.push(xpr);
            q.pop();
        }
    }
#endif
} // namespace riddle