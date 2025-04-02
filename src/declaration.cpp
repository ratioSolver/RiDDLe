#include "declaration.hpp"
#include "core.hpp"
#include "type.hpp"

namespace riddle
{
    void enum_declaration::declare(scope &scp) const
    {
        std::vector<expr> vals;
        for (const auto &val : values)
            vals.emplace_back(scp.get_core().new_string(std::string(val.value)));
        // we create the enum and add it to the scope..
        auto et = utils::make_u_ptr<enum_type>(scp, std::string(name.id), std::move(vals));

        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(std::move(et));
        else if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_type(std::move(et));
        else
            throw std::runtime_error("Invalid scope type");
    }
    void enum_declaration::refine(scope &scp) const
    {
        auto &et = static_cast<enum_type &>(scp.get_type(name.id)); // we retrieve the enum type.. we know it exists, because we declared it, so we can safely cast it..
        for (const auto &ref : enum_refs)
        {
            auto tp = &scp.get_type(ref[0].id);
            for (size_t i = 1; i < ref.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(tp))
                    tp = &ct->get_type(ref[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            if (auto etp = dynamic_cast<enum_type *>(tp))
                et.parents.emplace_back(*etp);
            else
                throw std::runtime_error("Invalid enum reference");
        }
    }

    void field_declaration::refine(scope &scp) const
    {
        auto *c_tp = &scp.get_type(tp[0].id);
        for (size_t i = 1; i < tp.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(c_tp))
                c_tp = &ct->get_type(tp[i].id);
            else
                throw std::runtime_error("Invalid type reference");

        for (const auto &f : fields)
            if (auto cr = dynamic_cast<core *>(&scp))
                cr->add_field(utils::make_u_ptr<field>(*c_tp, std::string(f.first.id), f.second));
            else if (auto ct = dynamic_cast<component_type *>(&scp))
                ct->add_field(utils::make_u_ptr<field>(*c_tp, std::string(f.first.id), f.second));
            else
                throw std::runtime_error("Invalid scope type");
    }

    void constructor_declaration::refine(scope &scp) const
    {
        std::vector<utils::u_ptr<field>> args; // the parameters of the constructor..
        args.reserve(this->params.size());
        for (const auto &param : this->params)
        {
            auto *c_tp = &scp.get_type(param.first[0].id);
            for (size_t i = 1; i < param.first.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(c_tp))
                    c_tp = &ct->get_type(param.first[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            args.emplace_back(utils::make_u_ptr<field>(*c_tp, std::string(param.second.id), nullptr));
        }

        if (auto tp = dynamic_cast<component_type *>(&scp))
            tp->add_constructor(utils::make_u_ptr<constructor>(*tp, std::move(args), inits, stmts));
        else
            throw std::runtime_error("Invalid scope type");
    }

    void method_declaration::refine(scope &scp) const
    {
        std::vector<utils::u_ptr<field>> args; // the parameters of the method..
        args.reserve(this->params.size());
        for (const auto &param : this->params)
        {
            auto *c_tp = &scp.get_type(param.second.id);
            for (size_t i = 0; i < param.first.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(c_tp))
                    c_tp = &ct->get_type(param.first[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            args.emplace_back(utils::make_u_ptr<field>(*c_tp, std::string(param.second.id), nullptr));
        }

        std::optional<utils::ref_wrapper<type>> rt;
        if (!this->rt.empty())
        {
            auto *c_tp = &scp.get_type(this->rt[0].id);
            for (size_t i = 1; i < this->rt.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(c_tp))
                    c_tp = &ct->get_type(this->rt[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            rt = *c_tp;
        }

        auto mthd = utils::make_u_ptr<method>(scp, rt, name.id, std::move(args), stmts);
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_method(std::move(mthd));
        else if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_method(std::move(mthd));
        else
            throw std::runtime_error("Invalid scope type");
    }

    void predicate_declaration::declare(scope &scp) const
    {
        auto pred = utils::make_u_ptr<predicate>(scp, std::string(name.id), std::vector<utils::u_ptr<field>>(), body);
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_predicate(std::move(pred));
        else if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_predicate(std::move(pred));
        else
            throw std::runtime_error("Invalid scope type");
    }
    void predicate_declaration::refine(scope &scp) const
    {
        auto &pred = static_cast<predicate &>(scp.get_predicate(name.id)); // we retrieve the predicate.. we know it exists, because we declared it, so we can safely cast it..
        for (const auto &par : params)
        {
            auto *c_tp = &scp.get_type(par.first[0].id);
            for (size_t i = 1; i < par.first.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(c_tp))
                    c_tp = &ct->get_type(par.first[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            auto arg = utils::make_u_ptr<field>(*c_tp, std::string(par.second.id), nullptr);
            pred.args.push_back(*arg);
            pred.add_field(std::move(arg));
        }
    }

    void class_declaration::declare(scope &scp) const
    { // we create the class and add it to the scope..
        auto new_ct = utils::make_u_ptr<component_type>(scp, std::string(name.id));
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(std::move(new_ct));
        else if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_type(std::move(new_ct));
        else
            throw std::runtime_error("Invalid scope type");

        // we declare the enclosed types..
        for (const auto &tp : types)
            tp->declare(*new_ct);
    }
    void class_declaration::refine(scope &scp) const
    {
        auto &ct = static_cast<component_type &>(scp.get_type(name.id)); // we retrieve the class type.. we know it exists, because we declared it, so we can safely cast it..
        for (const auto &base : base_classes)
        {
            auto tp = &scp.get_type(base[0].id);
            for (size_t i = 1; i < base.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(tp))
                    tp = &ct->get_type(base[i].id);
                else
                    throw std::runtime_error("Invalid type reference");
            if (auto ctp = dynamic_cast<component_type *>(tp))
                ct.parents.emplace_back(*ctp);
            else
                throw std::runtime_error("Invalid class reference");
        }

        // we refine the fields..
        for (const auto &field : fields)
            field->refine(ct);

        // we refine the constructors..
        for (const auto &constructor : constructors)
            constructor->refine(ct);

        // we refine the methods..
        for (const auto &method : methods)
            method->refine(ct);

        // we refine the (enclosed) types..
        for (const auto &tp : types)
            tp->refine(ct);

        // we declare the predicates..
        for (const auto &predicate : predicates)
            predicate->declare(ct);
    }
    void class_declaration::refine_predicates(scope &scp) const
    {
        auto &ct = static_cast<component_type &>(scp.get_type(name.id)); // we retrieve the class type.. we know it exists, because we declared it, so we can safely cast it..
        // we refine the predicates..
        for (const auto &predicate : predicates)
            predicate->refine(ct);
        // we refine the (enclosed) types..
        for (const auto &tp : types)
            tp->refine_predicates(ct);
    }
} // namespace riddle
