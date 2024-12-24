#include "core.hpp"
#include "item.hpp"
#include "type.hpp"
#include "parser.hpp"
#include <sstream>
#include <fstream>

namespace riddle
{
    core::core() noexcept : scope(*this, *this), env(*this, *this)
    {
        add_type(std::make_unique<bool_type>(*this));
        add_type(std::make_unique<int_type>(*this));
        add_type(std::make_unique<real_type>(*this));
        add_type(std::make_unique<time_type>(*this));
        add_type(std::make_unique<string_type>(*this));
    }

    void core::read(std::string &&script)
    {
        std::stringstream ss(script);
        parser p(ss);
        auto cu = p.parse_compilation_unit();
        cu->declare(*this);
        cu->refine(*this);
        cu->refine_predicates(*this);
        cu->execute(*this, *this);
        cus.push_back(std::move(cu));
    }

    void core::read(const std::vector<std::string> &files)
    {
        std::vector<std::unique_ptr<compilation_unit>> c_cus;
        c_cus.reserve(files.size());
        for (const auto &file : files)
            if (std::ifstream ifs(file); ifs.is_open())
            {
                parser p(ifs);
                c_cus.push_back(p.parse_compilation_unit());
            }
            else
                throw std::runtime_error("file `" + file + "` not found");

        for (auto &cu : c_cus)
            cu->declare(*this);
        for (auto &cu : c_cus)
            cu->refine(*this);
        for (auto &cu : c_cus)
            cu->refine_predicates(*this);
        for (auto &cu : c_cus)
            cu->execute(*this, *this);

        cus.insert(cus.end(), std::make_move_iterator(c_cus.begin()), std::make_move_iterator(c_cus.end()));
    }

    std::shared_ptr<bool_item> core::new_bool(const bool value) { return std::make_shared<bool_item>(static_cast<bool_type &>(get_type(bool_kw)), value ? utils::TRUE_lit : utils::FALSE_lit); }
    std::shared_ptr<arith_item> core::new_int(const INT_TYPE value) { return std::make_shared<arith_item>(static_cast<int_type &>(get_type(int_kw)), utils::lin(utils::rational(value))); }
    std::shared_ptr<arith_item> core::new_real(utils::rational &&value) { return std::make_shared<arith_item>(static_cast<real_type &>(get_type(real_kw)), utils::lin(value)); }
    std::shared_ptr<arith_item> core::new_time(utils::rational &&value) { return std::make_shared<arith_item>(static_cast<time_type &>(get_type(time_kw)), utils::lin(value)); }
    std::shared_ptr<string_item> core::new_string(std::string &&value) { return std::make_shared<string_item>(static_cast<string_type &>(get_type(string_kw)), std::move(value)); }

    field &core::get_field(std::string_view name) const
    {
        if (auto it = fields.find(name); it != fields.end())
            return *it->second;
        throw std::out_of_range("field `" + std::string(name) + "` not found");
    }

    method &core::get_method(std::string_view name, const std::vector<std::reference_wrapper<const type>> &argument_types) const
    {
        if (auto it = methods.find(name); it != methods.end())
            for (const auto &m : it->second)
                if (m->get_args().size() == argument_types.size())
                {
                    bool match = true;
                    for (size_t i = 0; i < argument_types.size(); ++i)
                        if (!m->get_field(m->get_args()[i]).get_type().is_assignable_from(argument_types[i].get()))
                        {
                            match = false;
                            break;
                        }
                    if (match)
                        return *m;
                }
        throw std::out_of_range("method `" + std::string(name) + "` not found");
    }
    type &core::get_type(std::string_view name) const
    {
        if (auto it = types.find(name); it != types.end())
            return *it->second;
        throw std::out_of_range("type `" + std::string(name) + "` not found");
    }
    predicate &core::get_predicate(std::string_view name) const
    {
        if (auto it = predicates.find(name); it != predicates.end())
            return *it->second;
        throw std::out_of_range("predicate `" + std::string(name) + "` not found");
    }

    std::shared_ptr<item> core::get(std::string_view name)
    {
        if (auto it = items.find(name); it != items.end())
            return it->second;
        throw std::out_of_range("item `" + std::string(name) + "` not found");
    }

    void core::add_type(std::unique_ptr<type> t)
    {
        std::string name = t->get_name();
        if (!types.emplace(name, std::move(t)).second)
            throw std::invalid_argument("type `" + name + "` already exists");
    }
} // namespace riddle