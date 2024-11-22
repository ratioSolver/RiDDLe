#include "lexer.hpp"
#include "logging.hpp"
#include <sstream>
#include <cmath>

namespace riddle
{
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
            match('b');
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
            match('c');
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
            match('e');
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
                        return make_id(finish_id(sb.substr(pos - 4, 3)));
                    if (is_alnum(ch))
                        return make_id(finish_id(sb.substr(pos - 5, 4)));
                    return make_token(FACT);
                case 'l':
                    match('l');
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
                match('o');
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
            match('g');
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
            match('i');
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
            match('o');
            if (!match('r'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 3, 2)));
            return make_token(OR);
        }
        break;
        case 'r':
        {
            match('r');
            if (!match('e'))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
            switch (ch)
            {
            case 'a':
                match('a');
                if (!match('l'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                return make_token(REAL);
            case 't':
                match('t');
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
            match('s');
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
            match('t');
            switch (ch)
            {
            case 'i':
                match('i');
                if (!match('m'))
                    return make_id(finish_id(sb.substr(pos - 3, 2)));
                if (!match('e'))
                    return make_id(finish_id(sb.substr(pos - 4, 3)));
                if (is_alnum(ch))
                    return make_id(finish_id(sb.substr(pos - 5, 4)));
                return make_token(TIME);
            case 'r':
                match('r');
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
            match('v');
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
            match(ch);
            unsigned int intgr_len = 1;
            while (is_digit(ch))
            {
                ++intgr_len;
                next();
            }
            if (ch == '.')
            {
                match('.');
                unsigned int dec_len = 0;
                while (is_digit(ch))
                {
                    ++dec_len;
                    next();
                }
                return make_real(utils::rational(static_cast<INT_TYPE>(std::stol(sb.substr(pos - intgr_len - dec_len - 2, intgr_len) + sb.substr(pos - dec_len - 1, dec_len))), static_cast<INT_TYPE>(std::pow(10, dec_len))));
            }
            return make_int(std::stoll(sb.substr(pos - intgr_len - 1, intgr_len)));
        }
        case '"':
        {
            match('"');
            unsigned int str_len = 0;
            while (ch != '"')
            {
                if (ch == -1)
                    return make_token(EoF);
                ++str_len;
                next();
            }
            match('"');
            return make_string(sb.substr(pos - str_len - 1, str_len));
        }
        case '.':
        {
            match('.');
            unsigned int dec_len = 0;
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
            match(',');
            return make_token(COMMA);
        case ':':
            match(':');
            return make_token(COLON);
        case ';':
            match(';');
            return make_token(SEMICOLON);
        case '(':
            match('(');
            return make_token(LPAREN);
        case ')':
            match(')');
            return make_token(RPAREN);
        case '[':
            match('[');
            return make_token(LBRACKET);
        case ']':
            match(']');
            return make_token(RBRACKET);
        case '{':
            match('{');
            return make_token(LBRACE);
        case '}':
            match('}');
            return make_token(RBRACE);
        case '+':
            match('+');
            return make_token(PLUS);
        case '-':
            match('-');
            return make_token(MINUS);
        case '*':
            match('*');
            return make_token(STAR);
        case '/':
            match('/');
            switch (ch)
            {
            case '/': // single-line comment
                while (ch != '\n' && ch != -1)
                    next();
                return next_token();
            case '*': // multi-line comment
                while (ch != -1)
                {
                    if (match('*') && match('/'))
                        return next_token();
                    next();
                }
                return make_token(EoF);
            default:
                return make_token(SLASH);
            }
        case '&':
            match('&');
            return make_token(AMP);
        case '|':
            match('|');
            return make_token(BAR);
        case '~':
            match('~');
            return make_token(TILDE);
        case '=':
            match('=');
            if (match('='))
                return make_token(EQEQ);
            return make_token(EQ);
        case '>':
            match('>');
            if (match('='))
                return make_token(GTEQ);
            return make_token(GT);
        case '<':
            match('<');
            if (match('='))
                return make_token(LTEQ);
            return make_token(LT);
        case '!':
            match('!');
            if (match('='))
                return make_token(BANGEQ);
            return make_token(BANG);
        case '^':
            match('^');
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
            next();
            if (is_alnum(ch))
                return make_id(finish_id(sb.substr(pos - 2, 1)));
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

    std::unique_ptr<token> lexer::make_token(symbol sym) noexcept { return std::make_unique<token>(sym, line, start_pos - 1, end_pos - 2); }
    std::unique_ptr<id_token> lexer::make_id(std::string &&id) noexcept { return std::make_unique<id_token>(std::move(id), line, start_pos - 1, end_pos - 2); }
    std::unique_ptr<bool_token> lexer::make_bool(bool value) noexcept { return std::make_unique<bool_token>(value, line, start_pos - 1, end_pos - 2); }
    std::unique_ptr<int_token> lexer::make_int(INT_TYPE value) noexcept { return std::make_unique<int_token>(value, line, start_pos - 1, end_pos - 2); }
    std::unique_ptr<real_token> lexer::make_real(utils::rational &&value) noexcept { return std::make_unique<real_token>(std::move(value), line, start_pos - 1, end_pos - 2); }
    std::unique_ptr<string_token> lexer::make_string(std::string &&value) noexcept { return std::make_unique<string_token>(std::move(value), line, start_pos - 1, end_pos - 2); }
} // namespace riddle
