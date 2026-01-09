#include "graph.hpp"
#include "items.hpp"
#include <algorithm>
#include <numeric>
#include <stack>

namespace riddle
{
    graph::graph(std::string_view name) noexcept : core(name) {}

    atom_state graph::get_atom_state(const atom_term &atm) const noexcept
    {
        switch (prop_val(static_cast<const atom &>(atm).get_sigma()))
        {
        case utils::True:
            return atom_state::active;
        case utils::False:
            return atom_state::unified;
        default:
            return atom_state::inactive;
        }
    }

    json::json graph::to_json() const
    {
        json::json j_graph = core::to_json();

        if (!flaws.empty())
        {
            json::json j_flaws;
            for (const auto &f : flaws)
                j_flaws[std::to_string(f->get_id())] = f->to_json();
            j_graph["flaws"] = std::move(j_flaws);
        }
        if (!resolvers.empty())
        {
            json::json j_resolvers;
            for (const auto &r : resolvers)
                j_resolvers[std::to_string(r->get_id())] = r->to_json();
            j_graph["resolvers"] = std::move(j_resolvers);
        }
        if (c_flaw)
            j_graph["current_flaw"] = c_flaw->get().get_id();
        if (c_res)
            j_graph["current_resolver"] = c_res->get().get_id();

        return j_graph;
    }

    void graph::add_causal_link(flaw &f, resolver &r) noexcept
    {
        f.supports.emplace_back(r);
        r.preconditions.emplace_back(f);
        new_clause({!r.get_rho(), f.get_phi()});
#ifdef RIDDLE_ENABLE_LISTENERS
        causal_link_added(f, r);
#endif
    }

    void graph::set_current_flaw(std::optional<std::reference_wrapper<flaw>> flw) noexcept
    {
        c_flaw = flw;
#ifdef RIDDLE_ENABLE_LISTENERS
        current_flaw(c_flaw);
#endif
    }
    void graph::set_current_resolver(std::optional<std::reference_wrapper<resolver>> res) noexcept
    {
        c_res = res;
#ifdef RIDDLE_ENABLE_LISTENERS
        current_resolver(c_res);
#endif
    }

    void graph::set_flaw_cost(flaw &flw, const utils::rational &cost) noexcept
    {
        flw.est_cost = cost;
#ifdef RIDDLE_ENABLE_LISTENERS
        flaw_cost_changed(flw);
#endif
        std::stack<std::pair<riddle::flaw *, std::unordered_set<riddle::flaw *>>> stk;
        stk.push({&flw, {}}); // we push the flaw in the stack..

        while (!stk.empty())
        {
            auto c_f = stk.top();
            stk.pop();

            utils::rational c_cost = utils::rational::positive_infinite;
            if (prop_val(c_f.first->get_phi()) != utils::False && c_f.second.insert(c_f.first).second) // we compute the cost of the flaw as the minimum of the costs of its resolvers..
                for (const auto &res : c_f.first->get_resolvers())
                    if (prop_val(res.get().get_rho()) != utils::False)
                        c_cost = std::min(c_cost, res.get().get_estimated_cost());

            if (c_f.first->get_estimated_cost() != c_cost) // we update the cost of the flaw..
            {
                c_f.first->est_cost = c_cost;
#ifdef RIDDLE_ENABLE_LISTENERS
                flaw_cost_changed(*c_f.first);
#endif

                // we propagate the cost to the supported resolvers..
                for (auto &support : c_f.first->get_supports())
                    stk.push({&support.get().get_flaw(), c_f.second}); // we push the supported flaw in the stack..
            }
        }
    }

    flaw::flaw(graph &gr, std::vector<std::reference_wrapper<resolver>> &&cs) : gr(gr), causes(std::move(cs))
    {
        for (auto &c : causes)
            supports.emplace_back(c.get());
    }

    utils::lit flaw::get_phi(const std::vector<std::reference_wrapper<riddle::resolver>> &causes) const noexcept
    {
        switch (causes.size())
        {
        case 0: // No causes, phi is always true..
            return utils::TRUE_lit;
        case 1: // Single cause, phi is the rho of the cause..
            return causes.front().get().get_rho();
        default: // Combine the causes' rhos into a single phi..
            auto phi = gr.new_prop();
            std::vector<utils::lit> rhos;
            for (auto &r : causes)
                rhos.push_back(!r.get().get_rho());
            rhos.push_back(phi);
            gr.new_clause(std::move(rhos));
            return phi;
        }
    }

    json::json flaw::to_json() const
    {
        json::json j_flaw{{"phi", utils::to_string(get_phi())}, {"cost", riddle::to_json(get_estimated_cost())}};
        if (!causes.empty())
        {
            json::json j_causes(json::json_type::array);
            for (const auto &c : causes)
                j_causes.push_back(c.get().get_id());
            j_flaw["causes"] = std::move(j_causes);
        }
        return j_flaw;
    }

    resolver::resolver(flaw &flw, const utils::lit &rho, utils::rational &&intrinsic_cost) : flw(flw), rho(rho), intrinsic_cost(intrinsic_cost)
    {
        if (rho != flw.get_phi())
            flw.gr.new_clause({!rho, flw.get_phi()});
    }

    utils::rational resolver::get_estimated_cost() const noexcept
    {
        if (preconditions.empty())
            return intrinsic_cost;
#ifdef H_ADD
        return std::accumulate(preconditions.begin(), preconditions.end(), intrinsic_cost, [](const auto &lhs, const auto &prec)
                               { return lhs + prec.get().get_estimated_cost(); });
#elif defined(H_MAX)
        return std::max_element(preconditions.begin(), preconditions.end(), [](const auto &lhs, const auto &rhs)
                                { return lhs.get().get_estimated_cost() < rhs.get().get_estimated_cost(); })
            ->get()
            .get_estimated_cost();
#else
        static_assert(false, "No heuristic defined for resolver cost estimation");
#endif
    }

    json::json resolver::to_json() const
    {
        json::json j_resolver{{"flaw", flw.get_id()}, {"rho", utils::to_string(rho)}, {"intrinsic_cost", riddle::to_json(intrinsic_cost)}};
        if (!preconditions.empty())
        {
            json::json j_preconditions(json::json_type::array);
            for (const auto &p : preconditions)
                j_preconditions.push_back(p.get().get_id());
            j_resolver["preconditions"] = std::move(j_preconditions);
        }
        return j_resolver;
    }

    bool have_common_ancestors(const flaw &a, const flaw &b)
    {
        if (&a == &b)
            return true;
        std::unordered_set<const flaw *> ancestors_a;

        std::stack<const flaw *> stk;
        stk.push(&a);
        while (!stk.empty())
        {
            const flaw *anc_f = stk.top();
            stk.pop();
            ancestors_a.insert(anc_f);
            for (const auto &r : anc_f->causes)
                stk.push(&r.get().get_flaw());
        }

        stk.push(&b);
        while (!stk.empty())
        {
            const flaw *anc_f = stk.top();
            stk.pop();
            if (ancestors_a.count(anc_f))
                return true;
            for (const auto &r : anc_f->causes)
                stk.push(&r.get().get_flaw());
        }
        return false;
    }
} // namespace riddle
