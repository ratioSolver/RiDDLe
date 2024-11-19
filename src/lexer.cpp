#include "lexer.hpp"
#include <sstream>

namespace riddle
{
    lexer::lexer(const std::string &source) : sb(source) { next(); }
    lexer::lexer(std::string &&source) : sb(std::move(source)) { next(); }
    lexer::lexer(std::istream &is) : sb(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()) { next(); }

    std::unique_ptr<token> lexer::next_token()
    {
        switch (ch)
        {
        case 'b':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('o'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('o'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('l'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(BOOL);
        }
        break;
        case 'i':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('n'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('t'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(INT);
        }
        break;
        case 'r':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('e'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('a'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('l'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(REAL);
        }
        break;
        case 's':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('t'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('r'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('i'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('n'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('g'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(STRING);
        }
        break;
        case 't':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('i'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('m'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('e'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(TIME);
        }
        break;
        case ' ':
        case '\t':
            next();
            return next_token();
        case '\n':
            ++line;
            [[fallthrough]];
        case '\r':
            start_pos = 0;
            end_pos = 0;
            next();
            return next_token();
        case ';':
            next();
            return make_token(SEMICOLON);
        default:
            if (is_alpha(ch))
            {
                std::string id;
                id.push_back(ch);
                return make_id(finish_id(id));
            }
            return make_token(EoF);
        }
    }

    bool lexer::match(char expected) noexcept
    {
        if (ch == expected)
        {
            next();
            return true;
        }
        return false;
    }

    char lexer::next() noexcept
    {
        if (pos < sb.size())
        {
            ch = sb[pos++];
            ++end_pos;
            return ch;
        }
        ++pos;
        ++end_pos;
        return ch = -1;
    }

    std::string lexer::finish_id(std::string &str) noexcept
    {
        while (is_alnum(ch))
        {
            str.push_back(ch);
            ch = next();
        }
        return str;
    }

    std::unique_ptr<token> lexer::make_token(symbol sym) noexcept
    {
        auto tk = std::make_unique<token>(sym, line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }

    std::unique_ptr<token> lexer::make_id(std::string &&id) noexcept
    {
        auto tk = std::make_unique<id_token>(std::move(id), line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }
} // namespace riddle
