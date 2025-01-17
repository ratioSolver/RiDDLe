#include "env.hpp"
#include "item.hpp"
#include "core.hpp"
#include <cassert>

namespace riddle
{
    env::env(core &c, env &parent, std::map<std::string, std::shared_ptr<item>, std::less<>> &&items) noexcept : cr(c), parent(parent), items(std::move(items)) {}

    std::shared_ptr<item> env::get(std::string_view name)
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else
            return parent.get(name);
    }

    json::json env::to_json() const
    {
        json::json j_itms;
        for (const auto &[name, itm] : items)
            if (itm->get_type().is_primitive()) // we add the json representation of the item..
                j_itms[name] = itm->to_json();
            else if (auto e = dynamic_cast<enum_item *>(itm.get()))
                j_itms[name] = e->to_json();
            else if (auto c = dynamic_cast<component *>(itm.get()))
                j_itms[name] = {{"type", "item"}, {"val", static_cast<uint64_t>(c->get_id())}};
            else if (auto a = dynamic_cast<atom *>(itm.get()))
                j_itms[name] = {{"type", "atom"}, {"val", static_cast<uint64_t>(a->get_id())}};
            else
                assert(false);
        return j_itms;
    }
} // namespace riddle