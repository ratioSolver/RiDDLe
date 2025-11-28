#include "flaw.hpp"
#include "resolver.hpp"
#include "core.hpp"
#include <stack>
#include <unordered_set>

namespace riddle
{
    flaw::flaw(core &cr, std::vector<std::shared_ptr<resolver>> &&cs) : cr(cr), causes(std::move(cs))
    {
        for (auto &c : causes)
            c->preconditions.push_back(shared_from_this()); // this flaw is a precondition of its `cause` cause..
    }

    json::json flaw::to_json() const
    {
        json::json j_flaw{{"cost", riddle::to_json(get_estimated_cost())}};
        if (!causes.empty())
        {
            json::json j_causes(json::json_type::array);
            for (const auto &c : causes)
                j_causes.push_back(c->get_id());
            j_flaw["causes"] = std::move(j_causes);
        }
        return j_flaw;
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
            for (const auto &r : anc_f->get_causes())
                stk.push(&r->get_flaw());
        }

        stk.push(&b);
        while (!stk.empty())
        {
            const flaw *anc_f = stk.top();
            stk.pop();
            if (ancestors_a.count(anc_f))
                return true;
            for (const auto &r : anc_f->get_causes())
                stk.push(&r->get_flaw());
        }
        return false;
    }
} // namespace riddle
