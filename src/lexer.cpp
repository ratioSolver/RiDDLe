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
            next();
            if (!match('u'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('t'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('o'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            return make_token(AUTO);
        }
        break;
        case 'b':
        {
            next();
            if (!match('o'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('o'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('l'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            return make_token(BOOL);
        }
        break;
        case 'c':
        {
            next();
            if (!match('l'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('a'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('s'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (!match('s'))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 6, 5)));
            return make_token(CLASS);
        }
        break;
        case 'e':
        {
            next();
            if (!match('n'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('u'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('m'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            return make_token(ENUM);
        }
        break;
        case 'f':
        {
            next();
            switch (ch)
            {
            case 'a':
                next();
                switch (ch)
                {
                case 'c':
                    next();
                    if (!match('t'))
                        return make_id(finish_id(sb.substr(pos - 4, 3)));
                    if (is_alnum(ch))
                        return make_id(finish_id(sb.substr(pos - 5, 4)));
                    return make_token(FACT);
                case 'l':
                    next();
                    if (!match('s'))
                        return make_id(finish_id(sb.substr(pos - 4, 3)));
                    if (!match('e'))
                        return make_id(finish_id(sb.substr(pos - 5, 4)));
                    if (is_alnum(ch))
                        return make_id(finish_id(sb.substr(pos - 6, 5)));
                    return make_bool(false);
                default:
                    return make_id(finish_id(sb.substr(pos - 3, 2)));
                }
                break;
            case 'o':
                if (!match('r'))
                    return make_id(finish_id(sb.substr(pos - 3, 2)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                return make_token(FOR);
            default:
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            }
        }
        break;
        case 'g':
        {
            next();
            if (!match('o'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('a'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('l'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            return make_token(GOAL);
        }
        break;
        case 'i':
        {
            next();
            if (!match('n'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('t'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            return make_token(INT);
        }
        break;
        case 'o':
        {
            next();
            if (!match('r'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            return make_token(OR);
        }
        break;
        case 'r':
        {
            next();
            if (!match('e'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            switch (ch)
            {
            case 'a':
                next();
                if (!match('l'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                return make_token(REAL);
            case 't':
                next();
                if (!match('u'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (!match('r'))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                if (!match('n'))
                    return make_id(finish_id(sb.substr(pos - 6, 5)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 7, 6)));
                return make_token(RETURN);
            default:
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            }
        }
        break;
        case 's':
        {
            next();
            if (!match('t'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('r'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('i'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (!match('n'))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
            if (!match('g'))
                return make_id(finish_id(sb.substr(pos - 6, 5)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 7, 6)));
            return make_token(STRING);
        }
        break;
        case 't':
        {
            next();
            switch (ch)
            {
            case 'i':
                next();
                if (!match('m'))
                    return make_id(finish_id(sb.substr(pos - 3, 2)));
                if (!match('e'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                return make_token(TIME);
            case 'r':
                next();
                if (!match('u'))
                    return make_id(finish_id(sb.substr(pos - 3, 2)));
                if (!match('e'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                return make_bool(true);
            default:
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            }
        }
        break;
        case 'v':
        {
            next();
            if (!match('o'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (!match('i'))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            if (!match('d'))
                return make_id(finish_id(sb.substr(pos - 4, 3)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 5, 4)));
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
            unsigned int intgr_len = 1;
            next();
            while (is_digit(ch))
            {
                ++intgr_len;
                next();
            }
            if (ch == '.')
            {
                unsigned int dec_len = 1;
                next();
                while (is_digit(ch))
                {
                    ++dec_len;
                    next();
                }
                return make_real(utils::rational(static_cast<INT_TYPE>(std::stol(sb.substr(pos - intgr_len - dec_len - 1, intgr_len + dec_len - 2)), static_cast<INT_TYPE>(std::pow(10, dec_len)))));
            }
            return make_int(std::stoll(sb.substr(pos - intgr_len, intgr_len - 1)));
        }
        case '"':
        {
            unsigned int str_len = 0;
            next();
            while (ch != '"')
            {
                if (ch == -1)
                    return make_token(EoF);
                ++str_len;
                next();
            }
            next();
            return make_string(sb.substr(pos - str_len - 1, str_len));
        }
        case '.':
        {
            unsigned int dec_len = 0;
            next();
            if (is_digit(ch))
            {
                while (is_digit(ch))
                {
                    ++dec_len;
                    next();
                }
                return make_real(utils::rational(static_cast<INT_TYPE>(std::stol(sb.substr(pos - dec_len - 1, dec_len))), static_cast<INT_TYPE>(std::pow(10, dec_len))));
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
                return make_id(finish_id(sb.substr(pos - 1, 1)));
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

    std::string lexer::finish_id(std::string &&str) noexcept
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
        auto tk = std::make_unique<id_token>(std::move(std::move(id)), line, start_pos, end_pos - 1);
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
