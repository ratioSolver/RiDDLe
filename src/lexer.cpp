#include "lexer.hpp"
#include "logging.hpp"
#include <sstream>
#include <cmath>

namespace riddle
{
    lexer::lexer(const std::string &source) : sb(source) { next(); }
    lexer::lexer(std::string &&source) : sb(std::move(source)) { next(); }
    lexer::lexer(std::istream &is) : sb(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()) { next(); }

    std::unique_ptr<token> lexer::next_token()
    {
        start_pos = end_pos;
        switch (ch)
        {
        case 'a':
        {
            match('a');
            if (!match('u'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('t'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('o'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(AUTO, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'b':
        {
            match('b');
            if (!match('o'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('o'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('l'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(BOOL, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'c':
        {
            match('c');
            if (!match('l'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('a'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('s'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (!match('s'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 6, 5)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(CLASS, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'e':
        {
            match('e');
            if (!match('n'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('u'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('m'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(ENUM, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'f':
        {
            match('f');
            switch (ch)
            {
            case 'a':
                match('a');
                switch (ch)
                {
                case 'c':
                    match('c');
                    if (!match('t'))
                        return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                    if (is_alnum(ch))
                        return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                    return std::make_unique<token>(FACT, line, start_pos - 1, end_pos - 2);
                case 'l':
                    match('l');
                    if (!match('s'))
                        return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                    if (!match('e'))
                        return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                    if (is_alnum(ch))
                        return std::make_unique<id_token>(finish_id(sb.substr(pos - 6, 5)), line, start_pos - 1, end_pos - 2);
                    return std::make_unique<bool_token>(false, line, start_pos - 1, end_pos - 2);
                default:
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
                }
                break;
            case 'o':
                match('o');
                if (!match('r'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
                if (is_alnum(ch))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                return std::make_unique<token>(FOR, line, start_pos - 1, end_pos - 2);
            default:
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            }
        }
        break;
        case 'g':
        {
            match('g');
            if (!match('o'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('a'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('l'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(GOAL, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'i':
        {
            match('i');
            if (!match('n'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('t'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(INT, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'o':
        {
            match('o');
            if (!match('r'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(OR, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 'r':
        {
            match('r');
            if (!match('e'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            switch (ch)
            {
            case 'a':
                match('a');
                if (!match('l'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                if (is_alnum(ch))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                return std::make_unique<token>(REAL, line, start_pos - 1, end_pos - 2);
            case 't':
                match('t');
                if (!match('u'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                if (!match('r'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                if (!match('n'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 6, 5)), line, start_pos - 1, end_pos - 2);
                if (is_alnum(ch))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 7, 6)), line, start_pos - 1, end_pos - 2);
                return std::make_unique<token>(RETURN, line, start_pos - 1, end_pos - 2);
            default:
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            }
        }
        break;
        case 's':
        {
            match('s');
            if (!match('t'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('r'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('i'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (!match('n'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            if (!match('g'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 6, 5)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 7, 6)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(STRING, line, start_pos - 1, end_pos - 2);
        }
        break;
        case 't':
        {
            match('t');
            switch (ch)
            {
            case 'i':
                match('i');
                if (!match('m'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
                if (!match('e'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                if (is_alnum(ch))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                return std::make_unique<token>(TIME, line, start_pos - 1, end_pos - 2);
            case 'r':
                match('r');
                if (!match('u'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
                if (!match('e'))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
                if (is_alnum(ch))
                    return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
                return std::make_unique<bool_token>(true, line, start_pos - 1, end_pos - 2);
            default:
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            }
        }
        break;
        case 'v':
        {
            match('v');
            if (!match('o'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 2, 1)), line, start_pos - 1, end_pos - 2);
            if (!match('i'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 3, 2)), line, start_pos - 1, end_pos - 2);
            if (!match('d'))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 4, 3)), line, start_pos - 1, end_pos - 2);
            if (is_alnum(ch))
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 5, 4)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(VOID, line, start_pos - 1, end_pos - 2);
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
                unsigned int dec_len = 0;
                next();
                while (is_digit(ch))
                {
                    ++dec_len;
                    next();
                }
                return std::make_unique<real_token>(utils::rational(static_cast<INT_TYPE>(std::stol(sb.substr(pos - intgr_len - dec_len - 2, intgr_len) + sb.substr(pos - intgr_len - 1, dec_len))), static_cast<INT_TYPE>(std::pow(10, dec_len))), line, start_pos - 1, end_pos - 2);
            }
            return std::make_unique<int_token>(std::stoll(sb.substr(pos - intgr_len - 1, intgr_len)), line, start_pos - 1, end_pos - 2);
        }
        case '"':
        {
            unsigned int str_len = 0;
            next();
            while (ch != '"')
            {
                if (ch == -1)
                    return std::make_unique<token>(EoF, line, start_pos - 1, end_pos - 2);
                ++str_len;
                next();
            }
            next();
            return std::make_unique<string_token>(sb.substr(pos - str_len - 1, str_len), line, start_pos - 1, end_pos - 2);
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
                return std::make_unique<real_token>(utils::rational(static_cast<INT_TYPE>(std::stol(sb.substr(pos - dec_len - 1, dec_len))), static_cast<INT_TYPE>(std::pow(10, dec_len))), line, start_pos - 1, end_pos - 2);
            }
            return std::make_unique<token>(DOT, line, start_pos - 1, end_pos - 2);
        }
        case ',':
            match(',');
            return std::make_unique<token>(COMMA, line, start_pos - 1, end_pos - 2);
        case ':':
            match(':');
            return std::make_unique<token>(COLON, line, start_pos - 1, end_pos - 2);
        case ';':
            match(';');
            return std::make_unique<token>(SEMICOLON, line, start_pos - 1, end_pos - 2);
        case '(':
            match('(');
            return std::make_unique<token>(LPAREN, line, start_pos - 1, end_pos - 2);
        case ')':
            match(')');
            return std::make_unique<token>(RPAREN, line, start_pos - 1, end_pos - 2);
        case '[':
            match('[');
            return std::make_unique<token>(LBRACKET, line, start_pos - 1, end_pos - 2);
        case ']':
            match(']');
            return std::make_unique<token>(RBRACKET, line, start_pos - 1, end_pos - 2);
        case '{':
            match('{');
            return std::make_unique<token>(LBRACE, line, start_pos - 1, end_pos - 2);
        case '}':
            match('}');
            return std::make_unique<token>(RBRACE, line, start_pos - 1, end_pos - 2);
        case '+':
            match('+');
            return std::make_unique<token>(PLUS, line, start_pos - 1, end_pos - 2);
        case '-':
            match('-');
            return std::make_unique<token>(MINUS, line, start_pos - 1, end_pos - 2);
        case '*':
            match('*');
            return std::make_unique<token>(STAR, line, start_pos - 1, end_pos - 2);
        case '/':
            match('/');
            return std::make_unique<token>(SLASH, line, start_pos - 1, end_pos - 2);
        case '&':
            match('&');
            return std::make_unique<token>(AMP, line, start_pos - 1, end_pos - 2);
        case '|':
            match('|');
            return std::make_unique<token>(BAR, line, start_pos - 1, end_pos - 2);
        case '~':
            match('~');
            return std::make_unique<token>(TILDE, line, start_pos - 1, end_pos - 2);
        case '=':
            match('=');
            if (match('='))
                return std::make_unique<token>(EQEQ, line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(EQ, line, start_pos - 1, end_pos - 2);
        case '>':
            match('>');
            if (match('='))
                return std::make_unique<token>(GTEQ, line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(GT, line, start_pos - 1, end_pos - 2);
        case '<':
            match('<');
            if (match('='))
                return std::make_unique<token>(LTEQ, line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(LT, line, start_pos - 1, end_pos - 2);
        case '!':
            match('!');
            if (match('='))
                return std::make_unique<token>(BANGEQ, line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(BANG, line, start_pos - 1, end_pos - 2);
        case '^':
            match('^');
            return std::make_unique<token>(CARET, line, start_pos - 1, end_pos - 2);
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
                return std::make_unique<id_token>(finish_id(sb.substr(pos - 1, 1)), line, start_pos - 1, end_pos - 2);
            return std::make_unique<token>(EoF, line, start_pos - 1, end_pos - 2);
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
} // namespace riddle
