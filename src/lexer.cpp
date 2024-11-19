#include "lexer.hpp"
#include <sstream>
#include <cmath>

namespace riddle
{
    lexer::lexer(const std::string &source) : sb(source) { next(); }
    lexer::lexer(std::string &&source) : sb(std::move(source)) { next(); }
    lexer::lexer(std::istream &is) : sb(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()) { next(); }

    std::unique_ptr<token> lexer::next_token()
    {
        switch (ch)
        {
        case 'a':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('u'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('t'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('o'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(AUTO);
        }
        break;
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
        case 'c':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('l'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('a'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('s'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('s'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(CLASS);
        }
        break;
        case 'e':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('n'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('u'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('m'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(ENUM);
        }
        break;
        case 'f':
        {
            std::string id;
            id.push_back(ch);
            next();
            switch (ch)
            {
            case 'a':
                id.push_back(ch);
                next();
                switch (ch)
                {
                case 'c':
                    id.push_back(ch);
                    if (!match('t'))
                        return make_id(finish_id(id));
                    if (is_alnum(ch))
                        return make_id(finish_id(id));
                    return make_token(FACT);
                case 'l':
                    id.push_back(ch);
                    if (!match('s'))
                        return make_id(finish_id(id));
                    id.push_back(ch);
                    if (!match('e'))
                        return make_id(finish_id(id));
                    if (is_alnum(ch))
                        return make_id(finish_id(id));
                    return make_bool(false);
                default:
                    return make_id(finish_id(id));
                }
                break;
            case 'o':
                id.push_back(ch);
                if (!match('r'))
                    return make_id(finish_id(id));
                if (is_alnum(ch))
                    return make_id(finish_id(id));
                return make_token(FOR);
            default:
                return make_id(finish_id(id));
            }
        }
        break;
        case 'g':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('o'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('a'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('l'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(GOAL);
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
        case 'o':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('r'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(OR);
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
            switch (ch)
            {
            case 'a':
                id.push_back(ch);
                if (!match('l'))
                    return make_id(finish_id(id));
                if (is_alnum(ch))
                    return make_id(finish_id(id));
                return make_token(REAL);
            case 't':
                id.push_back(ch);
                if (!match('u'))
                    return make_id(finish_id(id));
                id.push_back(ch);
                if (!match('r'))
                    return make_id(finish_id(id));
                id.push_back(ch);
                if (!match('n'))
                    return make_id(finish_id(id));
                if (is_alnum(ch))
                    return make_id(finish_id(id));
                return make_token(RETURN);
            default:
                return make_id(finish_id(id));
            }
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
            switch (ch)
            {
            case 'i':
                id.push_back(ch);
                if (!match('m'))
                    return make_id(finish_id(id));
                id.push_back(ch);
                if (!match('e'))
                    return make_id(finish_id(id));
                if (is_alnum(ch))
                    return make_id(finish_id(id));
                return make_token(TIME);
            case 'r':
                id.push_back(ch);
                if (!match('u'))
                    return make_id(finish_id(id));
                id.push_back(ch);
                if (!match('e'))
                    return make_id(finish_id(id));
                if (is_alnum(ch))
                    return make_id(finish_id(id));
                return make_bool(true);
            default:
                return make_id(finish_id(id));
            }
        }
        break;
        case 'v':
        {
            std::string id;
            id.push_back(ch);
            next();
            if (!match('o'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('i'))
                return make_id(finish_id(id));
            id.push_back(ch);
            if (!match('d'))
                return make_id(finish_id(id));
            if (is_alnum(ch))
                return make_id(finish_id(id));
            return make_token(VOID);
        }
        break;
        case '0': // in a number literal..
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            std::string intgr;
            intgr.push_back(ch);
            next();
            while (is_digit(ch))
            {
                intgr.push_back(ch);
                next();
            }
            if (ch == '.')
            {
                std::string dec;
                dec.push_back(ch);
                next();
                while (is_digit(ch))
                {
                    dec.push_back(ch);
                    next();
                }
                return make_real(utils::rational(static_cast<INT_TYPE>(std::stol(intgr + dec)), static_cast<INT_TYPE>(std::pow(10, dec.size()))));
            }
            return make_int(std::stoll(intgr));
        }
        case '"':
        {
            std::string str;
            next();
            while (ch != '"')
            {
                if (ch == -1)
                    return make_token(EoF);
                str.push_back(ch);
                next();
            }
            next();
            return make_string(std::move(str));
        }
        case '.':
        {
            std::string dec;
            dec.push_back(ch);
            next();
            if (is_digit(ch))
            {
                while (is_digit(ch))
                {
                    dec.push_back(ch);
                    next();
                }
                return make_real(utils::rational(0, static_cast<INT_TYPE>(std::pow(10, dec.size()))));
            }
            return make_token(DOT);
        }
        case ',':
            next();
            return make_token(COMMA);
        case ':':
            next();
            return make_token(COLON);
        case ';':
            next();
            return make_token(SEMICOLON);
        case '(':
            next();
            return make_token(LPAREN);
        case ')':
            next();
            return make_token(RPAREN);
        case '[':
            next();
            return make_token(LBRACKET);
        case ']':
            next();
            return make_token(RBRACKET);
        case '{':
            next();
            return make_token(LBRACE);
        case '}':
            next();
            return make_token(RBRACE);
        case '+':
            next();
            return make_token(PLUS);
        case '-':
            next();
            return make_token(MINUS);
        case '*':
            next();
            return make_token(STAR);
        case '/':
            next();
            return make_token(SLASH);
        case '&':
            next();
            return make_token(AMP);
        case '|':
            next();
            return make_token(BAR);
        case '~':
            next();
            return make_token(TILDE);
        case '=':
            next();
            if (match('='))
                return make_token(EQEQ);
            return make_token(EQ);
        case '>':
            next();
            if (match('='))
                return make_token(GTEQ);
            return make_token(GT);
        case '<':
            next();
            if (match('='))
                return make_token(LTEQ);
            return make_token(LT);
        case '!':
            next();
            if (match('='))
                return make_token(BANGEQ);
            return make_token(BANG);
        case '^':
            next();
            return make_token(CARET);
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
            next();
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

    std::unique_ptr<token> lexer::make_bool(bool value) noexcept
    {
        auto tk = std::make_unique<bool_token>(value, line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }

    std::unique_ptr<token> lexer::make_int(INT_TYPE value) noexcept
    {
        auto tk = std::make_unique<int_token>(value, line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }

    std::unique_ptr<token> lexer::make_real(utils::rational &&value) noexcept
    {
        auto tk = std::make_unique<real_token>(std::move(value), line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }

    std::unique_ptr<token> lexer::make_string(std::string &&value) noexcept
    {
        auto tk = std::make_unique<string_token>(std::move(value), line, start_pos, end_pos - 1);
        start_pos = end_pos;
        return tk;
    }
} // namespace riddle
