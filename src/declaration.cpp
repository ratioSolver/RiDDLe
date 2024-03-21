#include "declaration.hpp"
#include "core.hpp"
#include "method.hpp"

namespace riddle
{
    void typedef_declaration::declare(std::shared_ptr<scope> &scp) const
    { // we create the typedef and add it to the scope..
        auto td = std::make_unique<typedef_type>(scp, name.id, scp->get_core().get_type(primitive_type.id).value(), *expr);
        if (auto cr = std::dynamic_pointer_cast<core>(scp))
            cr->add_type(std::move(td));
        else if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_type(std::move(td));
        else
            throw std::runtime_error("invalid scope for type declaration");
    }

    void enum_declaration::declare(std::shared_ptr<scope> &scp) const
    {
        std::vector<std::shared_ptr<item>> vals;
        for (const auto &val : values)
            vals.push_back(scp->get_core().new_string(val.str));
        // we create the enum and add it to the scope..
        auto et = std::make_unique<enum_type>(scp, name.id, std::move(vals));

        if (auto cr = std::dynamic_pointer_cast<core>(scp))
            cr->add_type(std::move(et));
        else if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_type(std::move(et));
        else
            throw std::runtime_error("invalid scope for type declaration");
    }
    void enum_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        auto et = static_cast<enum_type *>(&scp->get_type(name.id).value().get()); // cast is safe because the type was declared in the same scope
        for (const auto &er : enum_refs)
        {
            auto t = scp->get_type(er[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(er[0].start_line) + ", " + std::to_string(er[0].start_pos) + "] type `" + er[0].id + "` not found");
            for (size_t i = 1; i < er.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(er[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(er[i].start_line) + ", " + std::to_string(er[i].start_pos) + "] type `" + er[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(er[i].start_line) + ", " + std::to_string(er[i].start_pos) + "] `" + er[i].id + "` is not a component type");
            if (auto c_et = dynamic_cast<enum_type *>(&t.value().get()))
                et->enums.emplace_back(*c_et);
            else
                throw std::runtime_error("`" + er[er.size() - 1].id + "` is not an enum type");
        }
    }

    void field_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        auto t = scp->get_type(type_ids[0].id);
        if (!t)
            throw std::runtime_error("[" + std::to_string(type_ids[0].start_line) + ", " + std::to_string(type_ids[0].start_pos) + "] type `" + type_ids[0].id + "` not found");
        for (size_t i = 1; i < type_ids.size(); ++i)
            if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
            {
                t = ct->get_type(type_ids[i].id);
                if (!t)
                    throw std::runtime_error("[" + std::to_string(type_ids[i].start_line) + ", " + std::to_string(type_ids[i].start_pos) + "] type `" + type_ids[i].id + "` not found");
            }
            else
                throw std::runtime_error("[" + std::to_string(type_ids[i].start_line) + ", " + std::to_string(type_ids[i].start_pos) + "] `" + type_ids[i].id + "` is not a component type");
        for (const auto &init_el : inits)
        {
        }
    }

    void constructor_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        std::vector<std::unique_ptr<field>> args; // the constructor's arguments
        args.reserve(parameters.size());
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp->get_type(tp_ids[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            args.push_back(std::make_unique<field>(t.value().get(), id.id));
        }

        // we create the constructor and add it to the scope..
        auto c = std::make_unique<constructor>(scp, std::move(args), inits, body);
        if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_constructor(std::move(c));
        else
            throw std::runtime_error("invalid scope for constructor declaration");
    }

    void method_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        std::optional<std::reference_wrapper<type>> rt; // the method's return type
        if (!return_type.empty())
        {
            auto t = scp->get_type(return_type[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(return_type[0].start_line) + ", " + std::to_string(return_type[0].start_pos) + "] type `" + return_type[0].id + "` not found");
            for (size_t i = 1; i < return_type.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(return_type[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(return_type[i].start_line) + ", " + std::to_string(return_type[i].start_pos) + "] type `" + return_type[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(return_type[i].start_line) + ", " + std::to_string(return_type[i].start_pos) + "] `" + return_type[i].id + "` is not a component type");
            rt = t.value().get();
        }

        std::vector<std::unique_ptr<field>> pars; // the method's arguments
        pars.reserve(parameters.size());
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp->get_type(tp_ids[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            pars.push_back(std::make_unique<field>(t.value().get(), id.id));
        }

        auto m = std::make_unique<method>(scp, rt, name.id, std::move(pars), std::move(body));
        if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_method(std::move(m));
        else if (auto cr = std::dynamic_pointer_cast<core>(scp))
            cr->add_method(std::move(m));
        else
            throw std::runtime_error("invalid scope for method declaration");
    }

    void predicate_declaration::declare(std::shared_ptr<scope> &scp) const
    { // we create the predicate and add it to the scope..
        auto p = std::make_unique<predicate>(scp, name.id, std::vector<std::unique_ptr<field>>(), body);
        if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_predicate(std::move(p));
        else if (auto cr = std::dynamic_pointer_cast<core>(scp))
            cr->add_predicate(std::move(p));
        else
            throw std::runtime_error("invalid scope for predicate declaration");
    }
    void predicate_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        auto p = static_cast<predicate *>(&scp->get_predicate(name.id).value().get()); // cast is safe because the predicate was declared in the same scope

        // the predicate's arguments
        for (const auto &[tp_ids, id] : parameters)
        {
            auto t = scp->get_type(tp_ids[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(tp_ids[0].start_line) + ", " + std::to_string(tp_ids[0].start_pos) + "] type `" + tp_ids[0].id + "` not found");
            for (size_t i = 1; i < tp_ids.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(tp_ids[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] type `" + tp_ids[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(tp_ids[i].start_line) + ", " + std::to_string(tp_ids[i].start_pos) + "] `" + tp_ids[i].id + "` is not a component type");
            p->add_field(std::make_unique<field>(t.value().get(), id.id));
        }
    }

    void class_declaration::declare(std::shared_ptr<scope> &scp) const
    { // we create the class and add it to the scope..
        auto c_ct = std::make_unique<component_type>(scp, name.id);
        if (auto cr = std::dynamic_pointer_cast<core>(scp))
            cr->add_type(std::move(c_ct));
        else if (auto ct = std::dynamic_pointer_cast<component_type>(scp))
            ct->add_type(std::move(c_ct));
        else
            throw std::runtime_error("invalid scope for type declaration");

        // we declare the enclosed types..
        for (const auto &tp : types)
            tp->declare(c_ct);
    }
    void class_declaration::refine(std::shared_ptr<scope> &scp) const
    {
        auto c_ct = static_cast<component_type *>(&scp->get_type(name.id).value().get()); // cast is safe because the type was declared in the same scope

        for (const auto &tp : base_classes)
        {
            auto t = scp->get_type(tp[0].id);
            if (!t)
                throw std::runtime_error("[" + std::to_string(tp[0].start_line) + ", " + std::to_string(tp[0].start_pos) + "] type `" + tp[0].id + "` not found");
            for (size_t i = 1; i < tp.size(); ++i)
                if (auto ct = dynamic_cast<component_type *>(&t.value().get()))
                {
                    t = ct->get_type(tp[i].id);
                    if (!t)
                        throw std::runtime_error("[" + std::to_string(tp[i].start_line) + ", " + std::to_string(tp[i].start_pos) + "] type `" + tp[i].id + "` not found");
                }
                else
                    throw std::runtime_error("[" + std::to_string(tp[i].start_line) + ", " + std::to_string(tp[i].start_pos) + "] `" + tp[i].id + "` is not a component type");
            c_ct->parents.emplace_back(t.value().get());
        }

        // we refine the fields..
        for (const auto &f : fields)
            f->refine(c_ct);
        // we refine the constructors..
        for (const auto &c : constructors)
            c->refine(c_ct);
        // we refine the methods..
        for (const auto &m : methods)
            m->refine(c_ct);
        // we refine the types..
        for (const auto &tp : types)
            tp->refine(c_ct);
        // we refine the predicates..
        for (const auto &p : predicates)
            p->refine(c_ct);
    }
    void class_declaration::refine_predicates(std::shared_ptr<scope> &scp) const
    {
        auto c_ct = static_cast<component_type *>(&scp->get_type(name.id).value().get()); // cast is safe because the type was declared in the same scope
        // we refine the predicates..
        for (const auto &p : predicates)
            p->refine(c_ct);
        // we refine the types..
        for (const auto &tp : types)
            tp->refine_predicates(c_ct);
    }
} // namespace riddle
