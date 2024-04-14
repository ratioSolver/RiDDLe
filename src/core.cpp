#include <algorithm>
#include <fstream>
#include "core.hpp"
#include "parser.hpp"

#ifdef ENABLE_VISUALIZATION
#include <queue>

#define RECOMPUTE_NAMES() recompute_names()
#else
#define RECOMPUTE_NAMES()
#endif

namespace riddle
{
    core::core() : scope(*this, *this), env(*this), bool_tp(static_cast<bool_type &>(*types.emplace("bool", std::make_unique<bool_type>(*this)).first->second)), int_tp(static_cast<int_type &>(*types.emplace("int", std::make_unique<int_type>(*this)).first->second)), real_tp(static_cast<real_type &>(*types.emplace("real", std::make_unique<real_type>(*this)).first->second)), time_tp(static_cast<time_type &>(*types.emplace("time", std::make_unique<time_type>(*this)).first->second)), string_tp(static_cast<string_type &>(*types.emplace("string", std::make_unique<string_type>(*this)).first->second)) {}

    void core::read(const std::string &script)
    {
        auto ctx = shared_from_this();
        parser p(script);
        auto cu = p.parse();
        cu->declare(*this);
        cu->refine(*this);
        cu->refine_predicates(*this);
        cu->execute(*this, ctx);
        cus.emplace_back(std::move(cu));
    }
    void core::read(const std::vector<std::string> &files)
    {
        auto ctx = shared_from_this();
        std::vector<std::unique_ptr<compilation_unit>> c_cus;
        c_cus.reserve(files.size());
        for (const auto &file : files)
            if (std::ifstream ifs(file); ifs)
            {
                parser p(ifs);
                auto cu = p.parse();
                c_cus.emplace_back(std::move(cu));
            }
            else
                throw std::invalid_argument("file `" + file + "` not found");

        for (auto &cu : c_cus)
            cu->declare(*this);
        for (auto &cu : c_cus)
            cu->refine(*this);
        for (auto &cu : c_cus)
            cu->refine_predicates(*this);
        for (auto &cu : c_cus)
            cu->execute(*this, ctx);

        cus.reserve(cus.size() + c_cus.size());
        for (auto &cu : c_cus)
            cus.emplace_back(std::move(cu));
    }

    std::shared_ptr<bool_item> core::new_bool(bool value) { return std::make_shared<bool_item>(bool_tp, value ? utils::TRUE_lit : utils::FALSE_lit); }

    std::shared_ptr<arith_item> core::new_int(INTEGER_TYPE value) { return std::make_shared<arith_item>(int_tp, utils::lin(utils::rational(value))); }
    std::shared_ptr<arith_item> core::new_real(const utils::rational &value) { return std::make_shared<arith_item>(real_tp, utils::lin(value)); }
    std::shared_ptr<arith_item> core::new_time(const utils::rational &value) { return std::make_shared<arith_item>(time_tp, utils::lin(value)); }

    std::shared_ptr<string_item> core::new_string() { return std::make_shared<string_item>(string_tp); }
    std::shared_ptr<string_item> core::new_string(const std::string &value) { return std::make_shared<string_item>(string_tp, value); }

    std::shared_ptr<component> core::new_item(component_type &tp) { return std::make_shared<component>(tp); }

    std::optional<std::reference_wrapper<field>> core::get_field(const std::string &name) const noexcept
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second.get();
        else
            return std::nullopt;
    }

    std::optional<std::reference_wrapper<method>> core::get_method(const std::string &name, const std::vector<std::reference_wrapper<const type>> &argument_types) const
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
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<type>> core::get_type(const std::string &name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        else
            return std::nullopt;
    }
    std::optional<std::reference_wrapper<predicate>> core::get_predicate(const std::string &name) const
    {
        if (auto it = predicates.find(name); it != predicates.end())
            return *it->second;
        else
            return std::nullopt;
    }

    std::shared_ptr<item> core::get(const std::string &name)
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        else
            return nullptr;
    }

    void core::add_type(std::unique_ptr<type> &&tp)
    {
        if (!types.emplace(tp->get_name(), std::move(tp)).second)
            throw std::runtime_error("type `" + tp->get_name() + "` already exists");
    }
    void core::add_predicate(std::unique_ptr<predicate> &&pred)
    {
        if (!predicates.emplace(pred->get_name(), std::move(pred)).second)
            throw std::runtime_error("predicate `" + pred->get_name() + "` already exists");
    }
    void core::add_method(std::unique_ptr<method> &&meth)
    {
        std::vector<std::reference_wrapper<const type>> args;
        for (const auto &arg : meth->get_arguments())
            args.push_back(arg.get().get_type());
        if (get_method(meth->get_name(), args))
            throw std::runtime_error("method `" + meth->get_name() + "` with the same arguments already exists");
        methods[meth->get_name()].emplace_back(std::move(meth));
    }

    type &determine_type(const std::vector<std::shared_ptr<arith_item>> &xprs)
    {
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_time(*x) || is_constant(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_time_type();
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_int(*x) || (is_real(*x) && is_constant(*x) && is_integer(x->get_type().get_scope().get_core().arithmetic_value(static_cast<arith_item &>(*x)))); }))
            return xprs[0]->get_type().get_scope().get_core().get_int_type();
        return xprs[0]->get_type().get_scope().get_core().get_real_type();
    }

#ifdef ENABLE_VISUALIZATION
    void core::recompute_names() noexcept
    {
        expr_names.clear();

        std::queue<std::pair<std::string, std::shared_ptr<item>>> q;
        for (const auto &xpr : get_items())
        {
            expr_names.emplace(xpr.second.get(), xpr.first);
            if (!xpr.second->get_type().is_primitive())
                q.push(xpr);
        }

        while (!q.empty())
        {
            const auto &c_xpr = q.front();
            if (const auto *c_env = dynamic_cast<env *>(c_xpr.second.get()))
                for (const auto &xpr : c_env->get_items())
                    if (expr_names.emplace(xpr.second.get(), expr_names.at(c_xpr.second.get()) + '.' + xpr.first).second)
                        if (!xpr.second->get_type().is_primitive())
                            q.push(xpr);
            q.pop();
        }
    }
#endif
} // namespace riddle