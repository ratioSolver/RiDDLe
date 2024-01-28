#include "constructor.hpp"

namespace riddle
{
    constructor::constructor(std::shared_ptr<scope> parent, std::vector<std::unique_ptr<field>> &&args) : scope(parent->get_core(), parent), args(std::move(args)) {}
} // namespace riddle