#include "constructor.hpp"

namespace riddle
{
    constructor::constructor(scope &scp, std::vector<std::unique_ptr<field>> &&args, std::vector<std::unique_ptr<statement>> &&body) noexcept : scope(scp.get_core(), scp), body(std::move(body))
    {
        for (auto &arg : args)
        {
            this->args.push_back(arg->get_name());
            add_field(std::move(arg));
        }
    }
} // namespace riddle
