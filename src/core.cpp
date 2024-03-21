#include <algorithm>
#include "core.hpp"
#include "parser.hpp"

namespace riddle
{
    core::core() : scope(*this), env(*this), bool_tp(*types.emplace("bool", std::make_unique<bool_type>(*this)).first->second), int_tp(*types.emplace("int", std::make_unique<int_type>(*this)).first->second), real_tp(*types.emplace("real", std::make_unique<real_type>(*this)).first->second), time_tp(*types.emplace("time", std::make_unique<time_type>(*this)).first->second), string_tp(*types.emplace("string", std::make_unique<string_type>(*this)).first->second) {}

    void core::read(const std::string &script)
    {
        auto scp = scope::shared_from_this();
        auto ctx = env::shared_from_this();
        parser p(script);
        auto cu = p.parse();
        cu->declare(scp);
        cu->refine(scp);
        cu->refine_predicates(scp);
        cu->execute(scp, ctx);
        cus.emplace_back(std::move(cu));
    }
    void core::read(const std::vector<std::string> &files)
    {
        auto scp = scope::shared_from_this();
        auto ctx = env::shared_from_this();
        std::vector<std::unique_ptr<compilation_unit>> c_cus;
        c_cus.reserve(files.size());
        for (const auto &file : files)
        {
            parser p(file);
            auto cu = p.parse();
            cu->declare(scp);
            cu->refine(scp);
            cu->refine_predicates(scp);
            cu->execute(scp, ctx);
            c_cus.emplace_back(std::move(cu));
        }

        cus.reserve(cus.size() + c_cus.size());
        for (auto &cu : c_cus)
            cus.emplace_back(std::move(cu));
    }

    std::shared_ptr<item> core::new_item(component_type &tp) { return std::make_shared<component>(tp); }

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

    std::shared_ptr<item> core::get(const std::string &name) const noexcept
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

    bool is_bool(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_bool_type() == &x.get_type(); }
    bool is_int(const riddle::item &x) noexcept
    {
        if (&x.get_type().get_scope().get_core().get_int_type() == &x.get_type())
            return true;
        if (x.get_type().get_scope().get_core().is_constant(x))
        {
            auto val = x.get_type().get_scope().get_core().arithmetic_value(x);
            return val.get_infinitesimal() == 0 && val.get_rational().denominator() == 1;
        }
        return false;
    }
    bool is_real(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_real_type() == &x.get_type() || x.get_type().get_scope().get_core().is_constant(x); }
    bool is_time(const riddle::item &x) noexcept { return &x.get_type().get_scope().get_core().get_time_type() == &x.get_type() || x.get_type().get_scope().get_core().is_constant(x); }

    type &determine_type(const std::vector<std::shared_ptr<item>> &xprs)
    {
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_int(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_int_type();
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_int(*x) || is_real(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_real_type();
        if (std::all_of(xprs.begin(), xprs.end(), [](const auto &x)
                        { return is_time(*x); }))
            return xprs[0]->get_type().get_scope().get_core().get_time_type();
        return xprs[0]->get_type();
    }
} // namespace riddle