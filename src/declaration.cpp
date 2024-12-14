#include "declaration.hpp"
#include "core.hpp"
#include "type.hpp"
#include <stdexcept>

namespace riddle
{
    void enum_declaration::declare(scope &scp) const
    {
        std::vector<std::shared_ptr<item>> vals;
        for (const auto &val : values)
            vals.emplace_back(scp.get_core().new_string(val.value));
        // we create the enum and add it to the scope..
        auto et = std::make_unique<enum_type>(scp, std::string(name.id), std::move(vals));

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
                et.enums.emplace_back(*etp);
            else
                throw std::runtime_error("Invalid enum reference");
        }
    }

    void field_declaration::refine(scope &scp) const
    {
    }

    void constructor_declaration::refine(scope &scp) const
    {
    }

    void method_declaration::refine(scope &scp) const
    {
    }

    void predicate_declaration::declare(scope &scp) const
    {
    }
    void predicate_declaration::refine(scope &scp) const
    {
    }

    void class_declaration::declare(scope &scp) const
    {
    }
    void class_declaration::refine(scope &scp) const
    {
    }
    void class_declaration::refine_predicates(scope &scp) const
    {
    }
} // namespace riddle
