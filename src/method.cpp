#include "method.hpp"

namespace riddle
{
    method::method(std::shared_ptr<scope> parent, std::shared_ptr<type> return_type, const std::string &name, std::vector<std::unique_ptr<field>> &&args) : scope(parent->get_core(), parent), return_type(return_type), name(name), args(std::move(args)) {}
} // namespace riddle