#include "parser.hpp"

namespace riddle
{
    std::unique_ptr<compilation_unit> parser::parse()
    {
        tk = next_token();
        return std::make_unique<compilation_unit>();
    }

    token *parser::next_token()
    {
        while (pos >= tokens.size())
        {
            auto c_tk = lex.next_token();
            tokens.emplace_back(std::move(c_tk));
        }
        return tokens[pos++].get();
    }

    bool parser::match(const symbol &sym)
    {
        if (tk->sym == sym)
        {
            tk = next_token();
            return true;
        }
        else
            return false;
    }

    void parser::backtrack(const size_t &p) noexcept
    {
        pos = p;
        tk = tokens[pos - 1].get();
    }

    void parser::error(const std::string &err)
    {
        std::string last_five;
        for (size_t i = pos - 5; i < pos; ++i)
            if (i < tokens.size())
                last_five += tokens[i]->to_string() + " ";
        throw std::invalid_argument("[" + std::to_string(tk->start_line + 1) + ", " + std::to_string(tk->start_pos + 1) + "] " + err + "\n" + last_five);
    }
} // namespace riddle