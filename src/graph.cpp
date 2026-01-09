#include "graph.hpp"
#include <algorithm>
#include <stack>

namespace riddle
{
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
#ifdef RIDDLE_ENABLE_LISTENERS
        causal_link_added(f, r);
#endif
    }

    void graph::compute_resolvers(flaw &flw) { flw.compute_resolvers(); }
    bool graph::apply_resolver(resolver &res, bool temp_res) noexcept
    {
        if (temp_res)
        {
            auto c = c_res;
            c_res = res;
            bool applied = res.apply();
            c_res = c;
            return applied;
        }
        else
            return res.apply();
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

    void graph::set_flaw_cost(flaw &flw, utils::rational &cost) noexcept
    {
        flw.est_cost = cost;
#ifdef RIDDLE_ENABLE_LISTENERS
        flaw_cost_changed(flw);
#endif
    }

    flaw::flaw(graph &gr, std::vector<std::reference_wrapper<resolver>> &&cs) : gr(gr), causes(std::move(cs))
    {
        for (auto &c : causes)
            supports.emplace_back(c.get());
    }

    void flaw::add_support(resolver &res) noexcept { gr.add_causal_link(*this, res); }

    json::json flaw::to_json() const
    {
        json::json j_flaw{{"cost", riddle::to_json(get_estimated_cost())}};
        if (!causes.empty())
        {
            json::json j_causes(json::json_type::array);
            for (const auto &c : causes)
                j_causes.push_back(c.get().get_id());
            j_flaw["causes"] = std::move(j_causes);
        }
        return j_flaw;
    }

    resolver::resolver(flaw &flw, utils::rational &&intrinsic_cost) : flw(flw), intrinsic_cost(intrinsic_cost) {}

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
        json::json j_resolver{{"flaw", flw.get_id()}, {"intrinsic_cost", riddle::to_json(intrinsic_cost)}};
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
