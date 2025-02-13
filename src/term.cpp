#include "term.hpp"

namespace riddle
{
    bool_expr push_negations(bool_expr expr) noexcept
    {
        if (auto not_xpr = utils::s_ptr_cast<bool_not>(expr))
        {
            if (auto not_xpr_xpr = utils::s_ptr_cast<bool_not>(not_xpr->arg))
                return push_negations(not_xpr_xpr->arg);
            else if (auto and_xpr = utils::s_ptr_cast<bool_and>(not_xpr->arg))
            {
                std::vector<bool_expr> args;
                for (const auto &arg : and_xpr->args)
                    args.push_back(push_negations(utils::make_s_ptr<bool_not>(and_xpr->cr, arg)));
                return utils::make_s_ptr<bool_or>(and_xpr->cr, std::move(args));
            }
            else if (auto or_xpr = utils::s_ptr_cast<bool_or>(not_xpr->arg))
            {
                std::vector<bool_expr> args;
                for (const auto &arg : or_xpr->args)
                    args.push_back(push_negations(utils::make_s_ptr<bool_not>(or_xpr->cr, arg)));
                return utils::make_s_ptr<bool_and>(or_xpr->cr, std::move(args));
            }
            else
                return not_xpr;
        }
        else if (auto and_xpr = utils::s_ptr_cast<bool_and>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : and_xpr->args)
                args.push_back(push_negations(arg));
            return utils::make_s_ptr<bool_and>(and_xpr->cr, std::move(args));
        }
        else if (auto or_xpr = utils::s_ptr_cast<bool_or>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : or_xpr->args)
                args.push_back(push_negations(arg));
            return utils::make_s_ptr<bool_or>(or_xpr->cr, std::move(args));
        }
        else
            return expr;
    }

    bool_expr distribute(bool_expr expr) noexcept
    {
        if (auto or_xpr = utils::s_ptr_cast<bool_or>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : or_xpr->args)
                args.push_back(distribute(arg));
            std::vector<bool_expr> new_args;
            for (const auto &arg : args)
                if (auto and_xpr = utils::s_ptr_cast<bool_and>(arg))
                    for (const auto &a : and_xpr->args)
                        new_args.push_back(a);
                else
                    new_args.push_back(arg);
            return utils::make_s_ptr<bool_or>(or_xpr->cr, std::move(new_args));
        }
        else if (auto and_xpr = utils::s_ptr_cast<bool_and>(expr))
        {
            std::vector<bool_expr> args;
            for (const auto &arg : and_xpr->args)
                args.push_back(distribute(arg));
            return utils::make_s_ptr<bool_and>(and_xpr->cr, std::move(args));
        }
        else
            return expr;
    }

    std::map<int_expr, INT_TYPE> linearize(int_expr expr)
    {
        std::map<int_expr, INT_TYPE> result;
        if (auto sum_xpr = utils::s_ptr_cast<int_add>(expr))
        {
            for (const auto &xpr : sum_xpr->args)
            {
                auto linear = linearize(xpr);
                for (const auto &[x, c] : linear)
                    result[x] += c;
            }
        }
        else if (auto sub_xpr = utils::s_ptr_cast<int_sub>(expr))
        {
            auto linear = linearize(sub_xpr->args[0]);
            for (const auto &[x, c] : linear)
                result[x] += c;
            linear = linearize(sub_xpr->args[1]);
            for (const auto &[x, c] : linear)
                result[x] -= c;
        }
        else if (auto prod_xpr = utils::s_ptr_cast<int_mul>(expr))
        {
            INT_TYPE c = 1;
            for (const auto &xpr : prod_xpr->args)
                if (auto int_xpr = utils::s_ptr_cast<int_const>(xpr))
                    c *= int_xpr->val.value;
                else
                {
                    auto linear = linearize(xpr);
                    for (const auto &[x, d] : linear)
                        if (auto it = result.find(x); it != result.end())
                            throw std::runtime_error("Non-linear expression");
                        else
                            result[x] = c * d;
                }
            if (result.empty())
                result[utils::s_ptr<int_term>()] = c;
        }
        else if (auto div_xpr = utils::s_ptr_cast<int_div>(expr))
        {
            INT_TYPE c = 1;
            for (const auto &xpr : prod_xpr->args)
                if (auto int_xpr = utils::s_ptr_cast<int_const>(xpr))
                    c /= int_xpr->val.value;
                else
                {
                    auto linear = linearize(xpr);
                    for (const auto &[x, d] : linear)
                        if (auto it = result.find(x); it != result.end())
                            throw std::runtime_error("Non-linear expression");
                        else
                            result[x] = c / d;
                }
            if (result.empty())
                result[utils::s_ptr<int_term>()] = c;
        }
        else
            result[expr] = 1;
        return result;
    }

    std::map<real_expr, utils::rational> linearize(real_expr expr)
    {
        std::map<real_expr, utils::rational> result;
        if (auto sum_xpr = utils::s_ptr_cast<real_add>(expr))
        {
            for (const auto &xpr : sum_xpr->args)
            {
                auto linear = linearize(xpr);
                for (const auto &[x, c] : linear)
                    result[x] += c;
            }
        }
        else if (auto sub_xpr = utils::s_ptr_cast<real_sub>(expr))
        {
            auto linear = linearize(sub_xpr->args[0]);
            for (const auto &[x, c] : linear)
                result[x] += c;
            linear = linearize(sub_xpr->args[1]);
            for (const auto &[x, c] : linear)
                result[x] -= c;
        }
        else if (auto prod_xpr = utils::s_ptr_cast<real_mul>(expr))
        {
            utils::rational c = utils::rational::one;
            for (const auto &xpr : prod_xpr->args)
                if (auto real_xpr = utils::s_ptr_cast<real_const>(xpr))
                    c *= real_xpr->val.value;
                else
                {
                    auto linear = linearize(xpr);
                    for (const auto &[x, d] : linear)
                        if (auto it = result.find(x); it != result.end())
                            throw std::runtime_error("Non-linear expression");
                        else
                            result[x] = c * d;
                }
            if (result.empty())
                result[utils::s_ptr<real_term>()] = c;
        }
        else if (auto div_xpr = utils::s_ptr_cast<real_div>(expr))
        {
            utils::rational c = utils::rational::one;
            for (const auto &xpr : prod_xpr->args)
                if (auto real_xpr = utils::s_ptr_cast<real_const>(xpr))
                    c /= real_xpr->val.value;
                else
                {
                    auto linear = linearize(xpr);
                    for (const auto &[x, d] : linear)
                        if (auto it = result.find(x); it != result.end())
                            throw std::runtime_error("Non-linear expression");
                        else
                            result[x] = c / d;
                }
            if (result.empty())
                result[utils::s_ptr<real_term>()] = c;
        }
        else
            result[expr] = utils::rational::one;
        return result;
    }
} // namespace riddle
