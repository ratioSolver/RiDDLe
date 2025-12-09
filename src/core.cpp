#include "core.hpp"
#include "timeline.hpp"
#include <sstream>
#include <fstream>
#include <queue>
#include <set>
#include <algorithm>
#include <cassert>

#ifdef COMPUTE_NAMES
#include <unordered_map>

#define RECOMPUTE_NAMES() recompute_names()
#else
#define RECOMPUTE_NAMES()
#endif

namespace riddle
{
    core::core(std::string_view name) noexcept : scope(*this, *this), env(*this, *this), name(name)
    {
        add_type(std::make_unique<bool_type>(*this));
        add_type(std::make_unique<int_type>(*this));
        add_type(std::make_unique<real_type>(*this));
        add_type(std::make_unique<time_type>(*this));
        add_type(std::make_unique<string_type>(*this));
    }

    void core::read(std::string_view script)
    {
        std::stringstream ss(script.data());
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
        std::vector<std::unique_ptr<compilation_unit>> c_cus;
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
        return std::make_shared<and_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_or(std::vector<bool_expr> &&exprs)
    {
        assert(!exprs.empty());
        return std::make_shared<or_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_xor(std::vector<bool_expr> &&exprs)
    {
        assert(!exprs.empty());
        return std::make_shared<xor_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(exprs));
    }

    bool_expr core::new_not(bool_expr expr) { return std::make_shared<bool_not>(static_cast<bool_type &>(get_type(bool_kw)), std::move(expr)); }

    bool_expr core::new_lt(arith_expr lhs, arith_expr rhs) { return std::make_shared<lt_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(lhs), std::move(rhs)); }
    bool_expr core::new_le(arith_expr lhs, arith_expr rhs) { return std::make_shared<le_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(lhs), std::move(rhs)); }
    bool_expr core::new_eq(expr lhs, expr rhs) { return std::make_shared<eq_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(lhs), std::move(rhs)); }
    bool_expr core::new_gt(arith_expr lhs, arith_expr rhs) { return std::make_shared<gt_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(lhs), std::move(rhs)); }
    bool_expr core::new_ge(arith_expr lhs, arith_expr rhs) { return std::make_shared<ge_term>(static_cast<bool_type &>(get_type(bool_kw)), std::move(lhs), std::move(rhs)); }

    bool core::assert_expr(bool_expr xpr) noexcept
    {
        if (auto n_xpr = std::dynamic_pointer_cast<bool_not>(xpr))
        { // we are dealing with a negation..
            if (auto lt_xpr = std::dynamic_pointer_cast<lt_term>(n_xpr->get_arg()))
                return mk_le(lt_xpr->get_rhs(), lt_xpr->get_lhs());
            else if (auto le_xpr = std::dynamic_pointer_cast<le_term>(n_xpr->get_arg()))
                return mk_lt(le_xpr->get_rhs(), le_xpr->get_lhs());
            else if (auto eq_xpr = std::dynamic_pointer_cast<eq_term>(n_xpr->get_arg()))
            {
                if (eq_xpr->get_lhs() == eq_xpr->get_rhs()) // the terms are the same, so they are equal..
                    return false;
                else if (&eq_xpr->get_lhs()->get_type() != &eq_xpr->get_lhs()->get_type()) // the types are different, so the constraint is always false..
                    return true;
                else if (auto lhs_xpr = std::dynamic_pointer_cast<arith_term>(eq_xpr->get_lhs())) // we are dealing with an arithmetic constraint..
                    return mk_neq(lhs_xpr, std::static_pointer_cast<arith_term>(eq_xpr->get_rhs()));
                else if (auto lhs_sxpr = std::dynamic_pointer_cast<string_term>(eq_xpr->get_lhs())) // we are dealing with a string constraint..
                    return mk_neq(lhs_sxpr, std::static_pointer_cast<string_term>(eq_xpr->get_rhs()));
                else if (auto lhs_bxpr = std::dynamic_pointer_cast<bool_term>(eq_xpr->get_lhs())) // we are dealing with a boolean constraint..
                    return mk_neq(lhs_bxpr, std::static_pointer_cast<bool_term>(eq_xpr->get_rhs()));
                else if (auto lhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_lhs())) // we are dealing with an enum constraint..
                {
                    if (auto rhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_rhs())) // both sides are enum items..
                        return mk_neq(lhs_enum_xpr, rhs_enum_xpr);
                    else // right side is an enum value..
                        return mk_forbid(lhs_enum_xpr, static_cast<utils::enum_val &>(*(eq_xpr->get_rhs())));
                }
                else if (auto rhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_rhs())) // left side is an enum value..
                    return mk_forbid(rhs_enum_xpr, static_cast<utils::enum_val &>(*(eq_xpr->get_lhs())));
                else if (auto lhs_atm = std::dynamic_pointer_cast<atom_term>(eq_xpr->get_lhs()))
                {
                    auto rhs_atm = std::static_pointer_cast<atom_term>(eq_xpr->get_rhs());
                    std::vector<bool_expr> clause_exprs;
                    std::queue<predicate *> q;
                    q.push(static_cast<predicate *>(&lhs_xpr->get_type()));
                    while (!q.empty())
                    {
                        for (const auto &[f_name, f] : q.front()->get_fields())
                            clause_exprs.push_back(new_not(new_eq(lhs_atm->get(f_name), rhs_atm->get(f_name))));
                        for (const auto &pp : q.front()->get_parents())
                            q.push(&pp.get());
                        q.pop();
                    }
                    new_clause(std::move(clause_exprs));
                    return true;
                }
                else
                    return true;
            }
            else if (auto ge_xpr = std::dynamic_pointer_cast<ge_term>(n_xpr->get_arg()))
                return mk_gt(ge_xpr->get_lhs(), ge_xpr->get_rhs());
            else if (auto gt_xpr = std::dynamic_pointer_cast<gt_term>(n_xpr->get_arg()))
                return mk_ge(gt_xpr->get_lhs(), gt_xpr->get_rhs());
            else if (auto b_xpr = std::dynamic_pointer_cast<bool_term>(n_xpr->get_arg()))
                return mk_assign(b_xpr, utils::False);
            else
                return false; // unsupported expression, just return false..
        }
        else
        {
            if (auto lt_xpr = std::dynamic_pointer_cast<lt_term>(xpr))
                return mk_lt(lt_xpr->get_lhs(), lt_xpr->get_rhs());
            else if (auto le_xpr = std::dynamic_pointer_cast<le_term>(xpr))
                return mk_le(le_xpr->get_lhs(), le_xpr->get_rhs());
            else if (auto eq_xpr = std::dynamic_pointer_cast<eq_term>(xpr))
            {
                if (eq_xpr->get_lhs() == eq_xpr->get_rhs()) // the terms are the same, so they are equal..
                    return true;
                else if (&eq_xpr->get_lhs()->get_type() != &eq_xpr->get_lhs()->get_type()) // the types are different, so the constraint is always false..
                    return false;
                else if (auto lhs_xpr = std::dynamic_pointer_cast<arith_term>(eq_xpr->get_lhs())) // we are dealing with an arithmetic constraint..
                    return mk_eq(lhs_xpr, std::static_pointer_cast<arith_term>(eq_xpr->get_rhs()));
                else if (auto lhs_sxpr = std::dynamic_pointer_cast<string_term>(eq_xpr->get_lhs())) // we are dealing with a string constraint..
                    return mk_eq(lhs_sxpr, std::static_pointer_cast<string_term>(eq_xpr->get_rhs()));
                else if (auto lhs_bxpr = std::dynamic_pointer_cast<bool_term>(eq_xpr->get_lhs())) // we are dealing with a boolean constraint..
                    return mk_eq(lhs_bxpr, std::static_pointer_cast<bool_term>(eq_xpr->get_rhs()));
                else if (auto lhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_lhs())) // we are dealing with an enum constraint..
                {
                    if (auto rhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_rhs())) // both sides are enum items..
                        return mk_eq(lhs_enum_xpr, rhs_enum_xpr);
                    else // right side is an enum value..
                        return mk_assign(lhs_enum_xpr, static_cast<utils::enum_val &>(*(eq_xpr->get_rhs())));
                }
                else if (auto rhs_enum_xpr = std::dynamic_pointer_cast<enum_term>(eq_xpr->get_rhs())) // left side is an enum value..
                    return mk_assign(rhs_enum_xpr, static_cast<utils::enum_val &>(*(eq_xpr->get_lhs())));
                else if (auto lhs_atm = std::dynamic_pointer_cast<atom_term>(eq_xpr->get_lhs()))
                {
                    auto rhs_atm = std::static_pointer_cast<atom_term>(eq_xpr->get_rhs());
                    std::queue<predicate *> q;
                    q.push(static_cast<predicate *>(&lhs_atm->get_type()));
                    while (!q.empty())
                    {
                        for (const auto &[f_name, f] : q.front()->get_fields())
                            if (!assert_expr(new_eq(lhs_atm->get(f_name), rhs_atm->get(f_name))))
                                return false;
                        for (const auto &pp : q.front()->get_parents())
                            q.push(&pp.get());
                        q.pop();
                    }
                    return true;
                }
                else
                    return false;
            }
            else if (auto ge_xpr = std::dynamic_pointer_cast<ge_term>(xpr))
                return mk_ge(ge_xpr->get_lhs(), ge_xpr->get_rhs());
            else if (auto gt_xpr = std::dynamic_pointer_cast<gt_term>(xpr))
                return mk_gt(gt_xpr->get_lhs(), gt_xpr->get_rhs());
            else if (auto b_xpr = std::dynamic_pointer_cast<bool_term>(xpr))
                return mk_assign(b_xpr, utils::True);
            else
                return false; // unsupported expression, just return false..
        }
    }

    atom_expr core::new_atom(bool is_fact, predicate &pred, std::map<std::string, expr, std::less<>> &&args)
    {
        auto atm = create_atom(is_fact, pred, std::move(args));

        // we add the atom to the predicate and to its parents..
        std::queue<predicate *> q;
        q.push(&pred);
        while (!q.empty())
        {
            auto p = q.front();
            p->atoms.push_back(atm);
            q.pop();
            for (auto &parent : p->parents)
                q.push(&parent.get());
        }

        // we add the atom to the predicate's scope, if a component-type, and to its parents..
        if (auto ct = dynamic_cast<component_type *>(&pred.get_scope()))
        {
            std::queue<component_type *> q;
            q.push(ct);
            while (!q.empty())
            {
                auto t = q.front();
                t->atoms.push_back(atm);
                t->created_atom(atm);
                q.pop();
                for (auto &parent : t->parents)
                    q.push(&parent.get());
            }
        }
        return atm;
    }

    field &core::get_field(std::string_view name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second;
        throw std::out_of_range("field `" + std::string(name) + "` not found");
    }

    method &core::get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const
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
        json::json j_core{{"name", name}};
        json::json j_timelines(json::json_type::array);

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
                    all_atoms.insert(static_cast<atom_term *>(atm.get()));
            for (const auto &etp : tp->get_types())
                if (auto ct = dynamic_cast<component_type *>(etp.second.get()))
                    q.push(ct);
                else if (auto et = dynamic_cast<enum_type *>(etp.second.get()))
                    for (const auto &val : et->get_domain())
                        all_items.insert(static_cast<term *>(val.get()));

            if (auto tl_tp = dynamic_cast<timeline *>(tp)) // we have a timeline type..
            {                                              // we extract the timeline..
                json::json j_tls = tl_tp->extract();
                for (size_t i = 0; i < j_tls.size(); ++i)
                    j_timelines.push_back(std::move(j_tls[i]));
            }
        }
        for (const auto &pred : predicates)
            for (const auto &atm : pred.second->get_atoms())
                all_atoms.insert(static_cast<atom_term *>(atm.get()));

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

        // for each pulse, the root atoms starting at that pulse..
        std::map<utils::inf_rational, std::set<atom_term *>> starting_atoms;
        // all the pulses of the solver timeline..
        std::set<utils::inf_rational> pulses;
        for (const auto &[_, pred] : get_predicates())
            if (pred->get_name() != impulse_kw && pred->get_name() != interval_kw)
                for (const auto &atm : pred->get_atoms())
                    if (get_atom_state(*atm) == atom_state::active)
                    { // we get only the active atoms..
                        if (get_predicate(impulse_kw).is_assignable_from(atm->get_type()))
                        { // we have an impulse atom..
                            const auto start = arith_value(*atm->get<arith_term>(at_kw));
                            starting_atoms[start].insert(static_cast<atom_term *>(&*atm));
                            pulses.insert(start);
                        }
                        else if (get_predicate(interval_kw).is_assignable_from(atm->get_type()))
                        { // we have an interval atom..
                            const auto start = arith_value(*atm->get<arith_term>(start_kw));
                            starting_atoms[start].insert(static_cast<atom_term *>(&*atm));
                            pulses.insert(start);
                        }
                    }
        if (!starting_atoms.empty())
        { // we have some root atoms in the solver timeline..
            json::json slv_tl{{"id", get_id()}, {"type", "Solver"}, {"name", get_name()}};
            json::json j_atms(json::json_type::array);
            for (const auto &p : pulses)
                for (const auto &atm : starting_atoms.at(p))
                    if (get_predicate(impulse_kw).is_assignable_from(atm->get_type()))
                        j_atms.push_back({{"atom", atm->get_id()}, {"at", riddle::to_json(arith_value(*atm->get<arith_term>(at_kw)))}});
                    else if (get_predicate(interval_kw).is_assignable_from(atm->get_type()))
                        j_atms.push_back({{"atom", atm->get_id()}, {"start", riddle::to_json(arith_value(*atm->get<arith_term>(start_kw)))}, {"end", riddle::to_json(arith_value(*atm->get<arith_term>(end_kw)))}});
            slv_tl["values"] = std::move(j_atms);
            j_timelines.push_back(std::move(slv_tl));
        }

        if (j_timelines.size() > 0)
            j_core["timelines"] = std::move(j_timelines);

        return j_core;
    }

    void core::add_method(std::unique_ptr<method> mthd)
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

    void core::add_predicate(std::unique_ptr<predicate> pred)
    {
        std::string name = pred->get_name();
        if (!predicates.emplace(name, std::move(pred)).second)
            throw std::invalid_argument("predicate `" + name + "` already exists");
    }

    void core::add_type(std::unique_ptr<type> t)
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

    json::json to_json(const utils::rational &r) noexcept { return json::json{{"num", r.numerator()}, {"den", r.denominator()}}; }

    json::json to_json(const utils::inf_rational &r) noexcept
    {
        json::json j = to_json(r.get_rational());
        if (!is_zero(r.get_infinitesimal()))
            j["inf"] = to_json(r.get_infinitesimal());
        return j;
    }
} // namespace riddle