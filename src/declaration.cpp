#include "declaration.hpp"
#include "core.hpp"
#include "method.hpp"

namespace riddle
{
    void typedef_declaration::declare(scope &scp) const
    { // we create the typedef and add it to the scope..
        auto td = std::make_unique<typedef_type>(scp, name.id, scp.get_core().get_type(primitive_type.id).value(), *expr);
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(std::move(td));
        else if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_type(std::move(td));
        else
            throw std::invalid_argument("Invalid scope for type declaration");
    }

    void enum_declaration::declare(scope &scp) const
    {
        std::vector<std::shared_ptr<item>> vals;
        for (const auto &val : values)
            vals.push_back(scp.get_core().new_string(val.str));
        // we create the enum and add it to the scope..
        auto et = std::make_unique<enum_type>(scp, name.id, std::move(vals));

        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(std::move(et));
        else if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_type(std::move(et));
        else
            throw std::invalid_argument("Invalid scope for type declaration");
    }
    void enum_declaration::refine(scope &scp) const
    {
        auto et = static_cast<enum_type *>(&scp.get_type(name.id)->get()); // cast is safe because the type was declared in the same scope
        for (const auto &er : enum_refs)
        {
            auto t = scp.get_type(er[0].id);
            if (!t)
                throw std::invalid_argument("[" + std::to_string(er[0].start_line) + ", " + std::to_string(er[0].start_pos) + "] type `" + er[0].id + "` not found");
            for (size_t i = 1; i < er.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(er[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(er[i].start_line) + ", " + std::to_string(er[i].start_pos) + "] type `" + er[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(er[i].start_line) + ", " + std::to_string(er[i].start_pos) + "] `" + er[i].id + "` is not a component type");
            if (auto c_et = dynamic_cast<enum_type *>(&t->get()))
                et->enums.emplace_back(*c_et);
            else
                throw std::invalid_argument("[" + std::to_string(er[er.size() - 1].start_line) + ", " + std::to_string(er[er.size() - 1].start_pos) + "] `" + er[er.size() - 1].id + "` is not an enum type");
        }
    }

    void field_declaration::refine(scope &scp) const
    {
        auto t = scp.get_type(type_ids[0].id);
        if (!t)
            throw std::invalid_argument("[" + std::to_string(type_ids[0].start_line) + ", " + std::to_string(type_ids[0].start_pos) + "] type `" + type_ids[0].id + "` not found");
        for (size_t i = 1; i < type_ids.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(&t->get()))
            {
                t = ct->get_type(type_ids[i].id);
                if (!t)
                    throw std::invalid_argument("[" + std::to_string(type_ids[i].start_line) + ", " + std::to_string(type_ids[i].start_pos) + "] type `" + type_ids[i].id + "` not found");
            }
            else
                throw std::invalid_argument("[" + std::to_string(type_ids[i].start_line) + ", " + std::to_string(type_ids[i].start_pos) + "] `" + type_ids[i].id + "` is not a component type");

        for (const auto &init : inits)
        { // we create the field and add it to the scope..
            auto f = std::make_unique<field>(t->get(), init.get_name().id, init.get_args());
            if (auto ct = dynamic_cast<component_type *>(&scp))
                ct->add_field(std::move(f));
            else if (auto cr = dynamic_cast<core *>(&scp))
                cr->add_field(std::move(f));
            else
                throw std::invalid_argument("Invalid scope for field declaration");
        }
    }

    void constructor_declaration::refine(scope &scp) const
    {
        std::vector<std::unique_ptr<field>> args; // the constructor's arguments
        args.reserve(parameters.size());
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp.get_type(tp_ids[0].id);
            if (!t)
                throw std::invalid_argument("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            args.push_back(std::make_unique<field>(t->get(), id.id));
        }

        // we create the constructor and add it to the scope..
        auto c = std::make_unique<constructor>(scp, std::move(args), inits, body);
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_constructor(std::move(c));
        else
            throw std::invalid_argument("Invalid scope for constructor declaration");
    }

    void method_declaration::refine(scope &scp) const
    {
        std::optional<std::reference_wrapper<type>> rt; // the method's return type
        if (!return_type.empty())
        {
            auto t = scp.get_type(return_type[0].id);
            if (!t)
                throw std::invalid_argument("[" + std::to_string(return_type[0].start_line) + ", " + std::to_string(return_type[0].start_pos) + "] type `" + return_type[0].id + "` not found");
            for (size_t i = 1; i < return_type.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(return_type[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(return_type[i].start_line) + ", " + std::to_string(return_type[i].start_pos) + "] type `" + return_type[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(return_type[i].start_line) + ", " + std::to_string(return_type[i].start_pos) + "] `" + return_type[i].id + "` is not a component type");
            rt = t->get();
        }

        std::vector<std::unique_ptr<field>> pars; // the method's arguments
        pars.reserve(parameters.size());
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp.get_type(tp_ids[0].id);
            if (!t)
                throw std::invalid_argument("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            pars.push_back(std::make_unique<field>(t->get(), id.id));
        }

        auto m = std::make_unique<method>(scp, rt, name.id, std::move(pars), std::move(body));
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_method(std::move(m));
        else if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_method(std::move(m));
        else
            throw std::invalid_argument("Invalid scope for method declaration");
    }

    void predicate_declaration::declare(scope &scp) const
    { // we create the predicate and add it to the scope..
        auto p = std::make_unique<predicate>(scp, name.id, std::vector<std::unique_ptr<field>>(), body);
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_predicate(std::move(p));
        else if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_predicate(std::move(p));
        else
            throw std::invalid_argument("Invalid scope for predicate declaration");
    }
    void predicate_declaration::refine(scope &scp) const
    {
        auto p = static_cast<predicate *>(&scp.get_predicate(name.id)->get()); // cast is safe because the predicate was declared in the same scope

        // the predicate's arguments
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp.get_type(tp_ids[0].id);
            if (!t)
                throw std::invalid_argument("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            p->add_field(std::make_unique<field>(t->get(), id.id));
        }

        // the predicate's parent predicates
        for (const auto &pr : base_predicates)
        {
            auto pr_pred = scp.get_predicate(pr[0].id);
            if (!pr_pred)
                throw std::invalid_argument("[" + std::to_string(pr[0].start_line) + ", " + std::to_string(pr[0].start_pos) + "] predicate `" + pr[0].id + "` not found");
            for (size_t i = 1; i < pr.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&pr_pred->get()))
                {
                    pr_pred = ct->get_predicate(pr[i].id);
                    if (!pr_pred)
                        throw std::invalid_argument("[" + std::to_string(pr[i].start_line) + ", " + std::to_string(pr[i].start_pos) + "] predicate `" + pr[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(pr[i].start_line) + ", " + std::to_string(pr[i].start_pos) + "] `" + pr[i].id + "` is not a component type");
            p->parents.emplace_back(*&pr_pred->get());
        }
    }

    void class_declaration::declare(scope &scp) const
    { // we create the class and add it to the scope..
        auto c_ct = std::make_unique<component_type>(scp, name.id);
        if (auto ct = dynamic_cast<component_type *>(&scp))
            ct->add_type(std::move(c_ct));
        else if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(std::move(c_ct));
        else
            throw std::invalid_argument("Invalid scope for type declaration");

        // we declare the enclosed types..
        for (const auto &tp : types)
            tp->declare(*c_ct);
    }
    void class_declaration::refine(scope &scp) const
    {
        auto c_ct = static_cast<component_type *>(&scp.get_type(name.id)->get()); // cast is safe because the type was declared in the same scope

        // we refine the base classes by adding the parent classes to the current class..
        for (const auto &tp : base_classes)
        {
            auto t = scp.get_type(tp[0].id); // the base class
            if (!t)
                throw std::invalid_argument("[" + std::to_string(tp[0].start_line) + ", " + std::to_string(tp[0].start_pos) + "] type `" + tp[0].id + "` not found");
            for (size_t i = 1; i < tp.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t->get()))
                {
                    t = ct->get_type(tp[i].id);
                    if (!t)
                        throw std::invalid_argument("[" + std::to_string(tp[i].start_line) + ", " + std::to_string(tp[i].start_pos) + "] type `" + tp[i].id + "` not found");
                }
                else
                    throw std::invalid_argument("[" + std::to_string(tp[i].start_line) + ", " + std::to_string(tp[i].start_pos) + "] `" + tp[i].id + "` is not a component type");

            if (auto p_ct = dynamic_cast<component_type *>(&t->get()))
                c_ct->parents.emplace_back(*p_ct);
            else
                throw std::invalid_argument("[" + std::to_string(tp[tp.size() - 1].start_line) + ", " + std::to_string(tp[tp.size() - 1].start_pos) + "] `" + tp[tp.size() - 1].id + "` is not a component type");
        }

        // we refine the fields..
        for (const auto &f : fields)
            f->refine(*c_ct);
        // we refine the constructors..
        for (const auto &c : constructors)
            c->refine(*c_ct);
        // we refine the methods..
        for (const auto &m : methods)
            m->refine(*c_ct);
        // we refine the (enclosed) types..
        for (const auto &tp : types)
            tp->refine(*c_ct);
        // we declare the (enclosed) predicates..
        for (const auto &p : predicates)
            p->declare(*c_ct);
    }
    void class_declaration::refine_predicates(scope &scp) const
    {
        auto c_ct = static_cast<component_type *>(&scp.get_type(name.id)->get()); // cast is safe because the type was declared in the same scope
        // we refine the predicates..
        for (const auto &p : predicates)
            p->refine(*c_ct);
        // we refine the types..
        for (const auto &tp : types)
            tp->refine_predicates(*c_ct);
    }
} // namespace riddle
