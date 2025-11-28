#include "types.hpp"
#include "items.hpp"
#include "core.hpp"
#include <sstream>
#include <set>

namespace riddle
{
    peak::peak(core &cr, std::vector<atom_expr> &&atms) : flaw(cr, causes_from_atoms(atms)) {}

    state_variable::state_variable(core &cr) noexcept : flaw_aware_component_type(cr, state_variable_kw), timeline(cr) { add_constructor(std::make_unique<constructor>(*this)); }

    void state_variable::created_predicate(predicate &pred) noexcept { add_parent(pred, get_core().get_predicate(interval_kw)); }

    std::vector<std::shared_ptr<flaw>> state_variable::get_flaws() noexcept
    {
        std::vector<std::shared_ptr<flaw>> flaws;
        // we partition atoms for each state-variable they might insist on..
        auto partition = partition_atoms();
        for (const auto &[sv, atms] : partition)
        {
            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);

            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                    overlapping_atoms.erase(a);

            for (p = std::next(p); p != pulses.end(); ++p)
            {
                if (overlapping_atoms.size() > 1)
                { // we have a peak..
                    std::vector<atom_expr> flaw_atms;
                    for (const auto &atm : overlapping_atoms)
                        flaw_atms.push_back(atm);
                    if (!flaw_atms.empty())
                        flaws.push_back(std::make_shared<peak>(get_core(), std::move(flaw_atms)));
                }

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                        overlapping_atoms.erase(a);
            }
        }
        return flaws;
    }

    json::json state_variable::extract() const
    {
        json::json tls(json::json_type::array);
        // we partition atoms for each state-variable they might insist on..
        auto partition = partition_atoms();
        for (const auto &[sv, atms] : partition)
        {
            json::json tl{{"id", static_cast<uint64_t>(sv->get_id())}, {"type", state_variable_kw}};
#ifdef COMPUTE_NAMES
            tl["name"] = guess_name(*sv);
#endif

            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);

            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                    overlapping_atoms.erase(a);

            json::json j_vals(json::json_type::array);
            for (p = std::next(p); p != pulses.end(); ++p)
            {
                json::json j_val;
                j_val[start_kw] = riddle::to_json(*std::prev(p));
                j_val[end_kw] = riddle::to_json(*p);

                json::json j_atms(json::json_type::array);
                for (const auto &atm : overlapping_atoms)
                    j_atms.push_back(static_cast<uint64_t>(atm->get_id()));
                j_val["atoms"] = std::move(j_atms);
                j_vals.push_back(std::move(j_val));

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                        overlapping_atoms.erase(a);
            }
            tl["values"] = std::move(j_vals);

            tls.push_back(std::move(tl));
        }

        return tls;
    }

    reusable_resource::reusable_resource(core &cr) noexcept : flaw_aware_component_type(cr, reusable_resource_kw), timeline(cr)
    {
        add_field(std::make_unique<field>(cr.get_type(real_kw), reusable_resource_capacity_kw, nullptr));

        std::istringstream ctr_decl("ReusableResource(real capacity) : capacity(capacity) { capacity >= 0.0; }");
        parser ctr_p(ctr_decl);
        ctr = ctr_p.parse_constructor_declaration();
        ctr->refine(*this);

        std::istringstream use_pred_decl("predicate Use(real amount) : Interval { amount >= 0.0; }");
        parser use_pred_p(use_pred_decl);
        use_pred = use_pred_p.parse_predicate_declaration();
        use_pred->declare(*this);
        use_pred->refine(*this);
    }

    void reusable_resource::created_predicate(predicate &pred) noexcept { add_parent(pred, get_core().get_predicate(interval_kw)); }

    std::vector<std::shared_ptr<flaw>> reusable_resource::get_flaws() noexcept
    {
        std::vector<std::shared_ptr<flaw>> flaws;
        // we partition atoms for each reusable-resource they might insist on..
        auto partition = partition_atoms();
        for (const auto &[rr, atms] : partition)
        {
            const auto c_capacity = get_core().arith_value(*rr->get<arith_term>(reusable_resource_capacity_kw));
            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);

            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                    overlapping_atoms.erase(a);

            for (p = std::next(p); p != pulses.end(); ++p)
            {
                utils::inf_rational c_usage; // the concurrent resource usage..
                for (const auto &atm : overlapping_atoms)
                    c_usage += get_core().arith_value(*atm->get<arith_term>(reusable_resource_amount_kw));
                if (c_usage > c_capacity)
                { // we have a peak..
                    std::vector<atom_expr> flaw_atms;
                    for (const auto &atm : overlapping_atoms)
                        flaw_atms.push_back(atm);
                    if (!flaw_atms.empty())
                        flaws.push_back(std::make_shared<peak>(get_core(), std::move(flaw_atms)));
                }

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                        overlapping_atoms.erase(a);
            }
        }
        return flaws;
    }

    json::json reusable_resource::extract() const
    {
        json::json tls(json::json_type::array);
        // we partition atoms for each reusable-resource they might insist on..
        auto partition = partition_atoms();
        for (const auto &[rr, atms] : partition)
        {
            json::json tl{{"id", static_cast<uint64_t>(rr->get_id())}, {"type", reusable_resource_kw}};
#ifdef COMPUTE_NAMES
            tl["name"] = guess_name(*rr);
#endif
            const auto c_capacity = get_core().arith_value(*rr->get<arith_term>(reusable_resource_capacity_kw));
            tl[reusable_resource_capacity_kw] = riddle::to_json(c_capacity);

            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);
            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                    overlapping_atoms.erase(a);

            json::json j_vals(json::json_type::array);
            for (p = std::next(p); p != pulses.end(); ++p)
            {
                json::json j_val;
                j_val[start_kw] = riddle::to_json(*std::prev(p));
                j_val[end_kw] = riddle::to_json(*p);

                json::json j_atms(json::json_type::array);
                utils::inf_rational c_usage; // the concurrent resource usage..
                for (const auto &atm : overlapping_atoms)
                {
                    c_usage += get_core().arith_value(*atm->get<arith_term>(reusable_resource_amount_kw));
                    j_atms.push_back(static_cast<uint64_t>(atm->get_id()));
                }
                j_val[reusable_resource_amount_kw] = riddle::to_json(c_usage);
                j_val["atoms"] = std::move(j_atms);
                j_vals.push_back(std::move(j_val));

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                        overlapping_atoms.erase(a);
            }
            tl["values"] = std::move(j_vals);

            tls.push_back(std::move(tl));
        }
        return tls;
    }

    consumable_resource::consumable_resource(core &cr) noexcept : flaw_aware_component_type(cr, consumable_resource_kw), timeline(cr)
    {
        add_field(std::make_unique<field>(cr.get_type(real_kw), consumable_resource_capacity_kw, nullptr));
        add_field(std::make_unique<field>(cr.get_type(real_kw), consumable_resource_initial_amount_kw, nullptr));

        std::istringstream ctr_decl("ConsumableResource(real capacity, real initial_amount) : capacity(capacity), initial_amount(initial_amount) { capacity >= 0.0; initial_amount <= capacity; }");
        parser ctr_p(ctr_decl);
        ctr = ctr_p.parse_constructor_declaration();
        ctr->refine(*this);

        std::istringstream prod_pred_decl("predicate Produce(real amount) : Interval { amount >= 0.0; }");
        parser prod_pred_p(prod_pred_decl);
        prod_pred = prod_pred_p.parse_predicate_declaration();
        prod_pred->declare(*this);
        prod_pred->refine(*this);

        std::istringstream cons_pred_decl("predicate Consume(real amount) : Interval { amount >= 0.0; }");
        parser cons_pred_p(cons_pred_decl);
        cons_pred = cons_pred_p.parse_predicate_declaration();
        cons_pred->declare(*this);
        cons_pred->refine(*this);
    }

    void consumable_resource::created_predicate(predicate &pred) noexcept { add_parent(pred, get_core().get_predicate(interval_kw)); }

    std::vector<std::shared_ptr<flaw>> consumable_resource::get_flaws() noexcept
    {
        std::vector<std::shared_ptr<flaw>> flaws;
        // we partition atoms for each consumable-resource they might insist on..
        auto partition = partition_atoms();
        for (const auto &[cr, atms] : partition)
        {
            const auto c_capacity = get_core().arith_value(*cr->get<arith_term>(consumable_resource_capacity_kw));
            const auto c_initial_amount = get_core().arith_value(*cr->get<arith_term>(consumable_resource_initial_amount_kw));
            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);

            std::vector<atom_expr> prods, consums;
            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                {
                    overlapping_atoms.erase(a);
                    if (get_predicate(consumable_resource_produce_predicate_kw).is_assignable_from(a->get_type()))
                        prods.push_back(a);
                    else
                        consums.push_back(a);
                }

            utils::inf_rational c_val = c_initial_amount;
            for (p = std::next(p); p != pulses.end(); ++p)
            {
                utils::inf_rational c_angular_coefficient; // the concurrent resource update..
                for (const auto &atm : overlapping_atoms)
                {
                    const auto c_amount = get_core().arith_value(*atm->get<arith_term>(consumable_resource_amount_kw));
                    auto c_coeff = get_predicate(consumable_resource_produce_predicate_kw).is_assignable_from(atm->get_type()) ? c_amount : -c_amount;
                    c_coeff /= (get_core().arith_value(*atm->get<arith_term>(start_kw)) - get_core().arith_value(*atm->get<arith_term>(end_kw))).get_rational();
                    c_angular_coefficient += c_coeff;
                }
                c_val += (c_angular_coefficient * (*p - *std::prev(p)).get_rational());
                if (c_val < utils::inf_rational::zero)
                { // we have a over-consumption..
                    std::vector<atom_expr> flaw_atms;
                    for (const auto &atm : consums)
                        flaw_atms.push_back(atm);
                    if (!flaw_atms.empty())
                        flaws.push_back(std::make_shared<overconsumption>(get_core(), std::move(flaw_atms)));
                }
                else if (c_val > c_capacity)
                { // we have a over-production..
                    std::vector<atom_expr> flaw_atms;
                    for (const auto &atm : prods)
                        flaw_atms.push_back(atm);
                    if (!flaw_atms.empty())
                        flaws.push_back(std::make_shared<overproduction>(get_core(), std::move(flaw_atms)));
                }

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                    {
                        overlapping_atoms.erase(a);
                        if (get_predicate(consumable_resource_produce_predicate_kw).is_assignable_from(a->get_type()))
                            prods.push_back(a);
                        else
                            consums.push_back(a);
                    }
            }
        }
        return flaws;
    }

    json::json consumable_resource::extract() const
    {
        json::json tls(json::json_type::array);
        // we partition atoms for each consumable-resource they might insist on..
        auto partition = partition_atoms();
        for (const auto &[cr, atms] : partition)
        {
            json::json tl{{"id", static_cast<uint64_t>(cr->get_id())}, {"type", consumable_resource_kw}};
#ifdef COMPUTE_NAMES
            tl["name"] = guess_name(*cr);
#endif
            const auto c_capacity = get_core().arith_value(*cr->get<arith_term>(consumable_resource_capacity_kw));
            tl[consumable_resource_capacity_kw] = riddle::to_json(c_capacity);
            const auto c_initial_amount = get_core().arith_value(*cr->get<arith_term>(consumable_resource_initial_amount_kw));
            tl[consumable_resource_initial_amount_kw] = riddle::to_json(c_initial_amount);

            auto [starting_atoms, ending_atoms, pulses] = get_pulses(atms);

            std::set<atom_expr> overlapping_atoms;
            std::set<utils::inf_rational>::iterator p = pulses.begin();
            if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
            if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                for (const auto &a : at_end_p->second)
                    overlapping_atoms.erase(a);

            json::json j_vals(json::json_type::array);
            utils::inf_rational c_val = c_initial_amount;
            for (p = std::next(p); p != pulses.end(); ++p)
            {
                json::json j_val;
                j_val[start_kw] = riddle::to_json(*std::prev(p));
                j_val[end_kw] = riddle::to_json(*p);

                json::json j_atms(json::json_type::array);
                utils::inf_rational c_angular_coefficient; // the concurrent resource update..
                for (const auto &atm : overlapping_atoms)
                {
                    const auto c_amount = get_core().arith_value(*atm->get<arith_term>(consumable_resource_amount_kw));
                    auto c_coeff = get_predicate(consumable_resource_produce_predicate_kw).is_assignable_from(atm->get_type()) ? c_amount : -c_amount;
                    c_coeff /= (get_core().arith_value(*atm->get<arith_term>(start_kw)) - get_core().arith_value(*atm->get<arith_term>(end_kw))).get_rational();
                    c_angular_coefficient += c_coeff;
                    j_atms.push_back(static_cast<uint64_t>(atm->get_id()));
                }
                j_val["from"] = riddle::to_json(c_val);
                c_val += (c_angular_coefficient * (*p - *std::prev(p)).get_rational());
                j_val["to"] = riddle::to_json(c_val);
                j_val["atoms"] = std::move(j_atms);
                j_vals.push_back(std::move(j_val));

                if (const auto at_start_p = starting_atoms.find(*p); at_start_p != starting_atoms.cend())
                    overlapping_atoms.insert(at_start_p->second.cbegin(), at_start_p->second.cend());
                if (const auto at_end_p = ending_atoms.find(*p); at_end_p != ending_atoms.cend())
                    for (const auto &a : at_end_p->second)
                        overlapping_atoms.erase(a);
            }
            tl["values"] = std::move(j_vals);

            tls.push_back(std::move(tl));
        }
        return tls;
    }

    [[nodiscard]] std::vector<std::shared_ptr<resolver>> causes_from_atoms(const std::vector<atom_expr> &atms) noexcept
    {
        std::vector<std::shared_ptr<resolver>> causes;
        for (const auto &atm : atms)
            for (const auto &c : atm->get_flaw().get_causes())
                causes.push_back(c);
        return causes;
    }
} // namespace riddle
