#include "lexer.hpp"
#include <sstream>

namespace riddle
{
    lexer::lexer(const std::string &source) : sb(source) { ch = next_char(); }
    lexer::lexer(std::istream &is) : sb(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()) { ch = next_char(); }

    std::unique_ptr<token> lexer::next_token()
    {
        switch (ch)
        {
        case '"':
        {
            // string literal..
            std::string str;
            while (true)
                switch (ch = next_char())
                {
                case '"':
                    ch = next_char();
                    return make_string_token(str);
                case '\\':
                    // read escaped char..
                    str += next_char();
                    break;
                case '\r':
                case '\n':
                    error("Newline in string literal..");
                    return nullptr;
                default:
                    str += ch;
                }
        }
        case '/':
            switch (ch = next_char())
            {
            case '/': // in single-line comment
                while (true)
                    switch (ch = next_char())
                    {
                    case '\r':
                    case '\n':
                        return next_token();
                    case -1:
                        return make_token(EOF_ID);
                    }
            case '*': // in multi-line comment
                while (true)
                    switch (ch = next_char())
                    {
                    case '*':
                        if ((ch = next_char()) == '/')
                        {
                            ch = next_char();
                            return next_token();
                        }
                        break;
                    }
            }
            return make_token(SLASH_ID);
        case '=':
            if ((ch = next_char()) == '=')
            {
                ch = next_char();
                return make_token(EQEQ_ID);
            }
            return make_token(EQ_ID);
        case '>':
            if ((ch = next_char()) == '=')
            {
                ch = next_char();
                return make_token(GTEQ_ID);
            }
            return make_token(GT_ID);
        case '<':
            if ((ch = next_char()) == '=')
            {
                ch = next_char();
                return make_token(LTEQ_ID);
            }
            return make_token(LT_ID);
        case '+':
            ch = next_char();
            return make_token(PLUS_ID);
        case '-':
            if ((ch = next_char()) == '>')
            {
                ch = next_char();
                return make_token(IMPLICATION_ID);
            }
            return make_token(MINUS_ID);
        case '*':
            ch = next_char();
            return make_token(STAR_ID);
        case '|':
            ch = next_char();
            return make_token(BAR_ID);
        case '&':
            ch = next_char();
            return make_token(AMP_ID);
        case '^':
            ch = next_char();
            return make_token(CARET_ID);
        case '!':
            if ((ch = next_char()) == '=')
            {
                ch = next_char();
                return make_token(BANGEQ_ID);
            }
            return make_token(BANG_ID);
        case '.':
            ch = next_char();
            if ('0' <= ch && ch <= '9')
            {
                // in a number literal..
                std::string dec;
                dec += ch;
                while (true)
                {
                    switch (ch = next_char())
                    {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        dec += ch;
                        break;
                    case '.':
                        error("Invalid numeric literal..");
                        return nullptr;
                    default:
                        return make_real_token("", dec);
                    }
                }
            }
            return make_token(DOT_ID);
        case ',':
            ch = next_char();
            return make_token(COMMA_ID);
        case ';':
            ch = next_char();
            return make_token(SEMICOLON_ID);
        case ':':
            ch = next_char();
            return make_token(COLON_ID);
        case '(':
            ch = next_char();
            return make_token(LPAREN_ID);
        case ')':
            ch = next_char();
            return make_token(RPAREN_ID);
        case '[':
            ch = next_char();
            return make_token(LBRACKET_ID);
        case ']':
            ch = next_char();
            return make_token(RBRACKET_ID);
        case '{':
            ch = next_char();
            return make_token(LBRACE_ID);
        case '}':
            ch = next_char();
            return make_token(RBRACE_ID);
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
            std::string intgr; // the integer part..
            intgr += ch;
            while (true)
                switch (ch = next_char())
                {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    intgr += ch;
                    break;
                case '.':
                {
                    std::string dcml; // the decimal part..
                    while (true)
                        switch (ch = next_char())
                        {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            dcml += ch;
                            break;
                        case '.':
                            error("Invalid numeric literal..");
                            return nullptr;
                        default:
                            return make_real_token(intgr, dcml);
                        }
                }
                default:
                    return make_int_token(intgr);
                }
        }
        case 'b':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'o')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'o')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'l')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(BOOL_ID);
        }
        case 'c':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'l')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'a')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 's')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 's')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(CLASS_ID);
        }
        case 'e':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'n')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'u')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'm')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(ENUM_ID);
        }
        case 'f':
        {
            std::string str;
            switch (str += ch; ch = next_char())
            {
            case 'a':
                switch (str += ch; ch = next_char())
                {
                case 'c':
                    if (str += ch; (ch = next_char()) != 't')
                        return finish_id(str);
                    if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                        return finish_id(str);
                    else
                        return make_token(FACT_ID);
                case 'l':
                    if (str += ch; (ch = next_char()) != 's')
                        return finish_id(str);
                    if (str += ch; (ch = next_char()) != 'e')
                        return finish_id(str);
                    if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                        return finish_id(str);
                    else
                        return make_bool_token(false);
                default:
                    return finish_id(str);
                }
            case 'o':
                if (str += ch; (ch = next_char()) != 'r')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_token(FOR_ID);
            default:
                return finish_id(str);
            }
        }
        case 'g':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'o')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'a')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'l')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(GOAL_ID);
        }
        case 'i':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'n')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 't')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(INT_ID);
        }
        case 'n':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'e')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'w')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(NEW_ID);
        }
        case 'o':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'r')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(OR_ID);
        }
        case 'p':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'r')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'e')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'd')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'i')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'c')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'a')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 't')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'e')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(PREDICATE_ID);
        }
        case 'r':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'e')
                return finish_id(str);
            switch (str += ch; ch = next_char())
            {
            case 'a':
                if (str += ch; (ch = next_char()) != 'l')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_token(REAL_ID);
            case 't':
                if (str += ch; (ch = next_char()) != 'u')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'r')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'n')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_token(RETURN_ID);
            default:
                return finish_id(str);
            }
        }
        case 's':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 't')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'r')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'i')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'n')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'g')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(STRING_ID);
        }
        case 't':
        {
            std::string str;
            switch (str += ch; ch = next_char())
            {
            case 'i':
                if (str += ch; (ch = next_char()) != 'm')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'e')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_token(TIME_ID);
            case 'r':
                if (str += ch; (ch = next_char()) != 'u')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'e')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_bool_token(true);
            case 'y':
                if (str += ch; (ch = next_char()) != 'p')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'e')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'd')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'e')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != 'f')
                    return finish_id(str);
                if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                    return finish_id(str);
                else
                    return make_token(TYPEDEF_ID);
            default:
                return finish_id(str);
            }
        }
        case 'v':
        {
            std::string str;
            if (str += ch; (ch = next_char()) != 'o')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'i')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != 'd')
                return finish_id(str);
            if (str += ch; (ch = next_char()) != -1 && is_id_part(ch))
                return finish_id(str);
            else
                return make_token(VOID_ID);
        }
        case 'a':
        case 'd':
        case 'h':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'q':
        case 'u':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        {
            std::string str;
            return finish_id(str);
        }
        case '\t':
        case ' ':
        case '\r':
        case '\n':
            while (true)
                switch (ch = next_char())
                {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    break;
                case -1:
                    return make_token(EOF_ID);
                default:
                    return next_token();
                }
        case -1:
            return make_token(EOF_ID);
        default:
            error("Unknown character: " + std::to_string(ch));
            return nullptr;
        }
    }

    char lexer::next_char()
    {
        if (pos == sb.length())
            return -1;
        switch (sb[pos])
        {
        case ' ':
            start_pos++;
            end_pos++;
            break;
        case '\t':
            start_pos += 4 - (start_pos % 4);
            end_pos += 4 - (end_pos % 4);
            break;
        case '\r':
            if (pos + 1 != sb.length() && sb[pos + 1] == '\n')
            {
                pos++;
                end_line++;
                end_pos = 0;
                break;
            }
            [[fallthrough]];
        case '\n':
            end_line++;
            end_pos = 0;
            break;
        default:
            end_pos++;
            break;
        }
        return sb[pos++];
    }

    std::unique_ptr<token> lexer::finish_id(std::string &str) noexcept
    {
        if (!is_id_part(ch))
            return make_id_token(str);
        str += ch;
        while ((ch = next_char()) != -1 && is_id_part(ch))
            str += ch;
        return make_id_token(str);
    }

    void lexer::error(const std::string &err) { throw std::invalid_argument("[" + std::to_string(start_line) + ", " + std::to_string(start_pos) + "] " + err); }
} // namespace riddle
