#include "constructor.hpp"

namespace riddle
{
    constructor::constructor(core &c, scope &parent, std::vector<std::unique_ptr<field>> &&args) noexcept : scope(c, parent)
    {
        for (auto &arg : args)
        {
            this->args.push_back(arg->get_name());
            add_field(std::move(arg));
        }
    }
} // namespace riddle
