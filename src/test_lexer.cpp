#include "test_lexer.hpp"
#include <stdexcept>
#include <cmath>

namespace riddle
{
    std::vector<std::unique_ptr<token>> lexer::parse(std::istream &is)
    {
        std::vector<std::unique_ptr<token>> tokens;

        current_state = START;
        line = 1;
        start = 0;
        text.clear();

        while (is.good())
        {
            switch (auto c = is.get())
            {
            case '/':
                switch (is.peek())
                {
                case '/': // single line comment
                    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    ++line;
                    start = 0;
                    break;
                case '*': // multi-line comment
                    is.ignore();
                    while (is.good())
                    {
                        if (is.get() == '*' && is.peek() == '/')
                        {
                            is.ignore();
                            break;
                        }
                        if (is.peek() == '\n')
                            ++line;
                    }
                    start = 0;
                    break;
                default:
                    text.push_back(c);
                    current_state = SLASH;
                    tokens.push_back(finish_token());
                    text.clear();
                }
                break;
            case 'b':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = BOOL;
                }
                else
                    text.push_back(c);
                break;
            case 'c':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = CLASS;
                }
                else
                    text.push_back(c);
                break;
            case 'e':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = ENUM;
                }
                else
                    text.push_back(c);
                break;
            case 'f':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = FOR;
                }
                else
                    text.push_back(c);
                break;
            case 'g':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = GOAL;
                }
                else
                    text.push_back(c);
                break;
            case 'h':
                if (current_state == TIME && text == "t") // th
                {
                    text.push_back(c);
                    current_state = THIS;
                }
                else
                    text.push_back(c);
                break;
            case 'i':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = INT;
                }
                else
                    text.push_back(c);
                break;
            case 'l':
                if (current_state == FACT)
                {
                    text.push_back(c);
                    current_state = Bool;
                }
                else
                    text.push_back(c);
                break;
            case 'n':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = NEW;
                }
                else
                    text.push_back(c);
                break;
            case 'o':
                switch (current_state)
                {
                case START:
                    text.push_back(c);
                    current_state = OR;
                    break;
                default:
                    text.push_back(c);
                }
                break;
            case 'p':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = PREDICATE;
                }
                else
                    text.push_back(c);
                break;
            case 'r':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = REAL;
                }
                else if (current_state == TIME && text == "t") // tr
                {
                    text.push_back(c);
                    current_state = Bool;
                }
                else
                    text.push_back(c);
                break;
            case 's':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = STRING;
                }
                else
                    text.push_back(c);
                break;
            case 't':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = TIME;
                }
                else if (current_state == REAL && text == "re") // ret
                {
                    text.push_back(c);
                    current_state = RETURN;
                }
                else
                    text.push_back(c);
                break;
            case 'v':
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = VOID;
                }
                else
                    text.push_back(c);
                break;
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
                if (current_state == START)
                {
                    text.push_back(c);
                    current_state = Int;
                }
                else
                    text.push_back(c);
                break;
            case '.':
                switch (current_state)
                {
                case START:
                    if (std::isdigit(is.peek()))
                    {
                        text.push_back(c);
                        current_state = Real;
                    }
                    else
                    {
                        if (!text.empty())
                        {
                            tokens.push_back(finish_token());
                            text.clear();
                        }
                        text.push_back(c);
                        current_state = DOT;
                        tokens.push_back(finish_token());
                        text.clear();
                    }
                    break;
                case Int:
                    text.push_back(c);
                    current_state = Real;
                    break;
                default:
                    text.push_back(c);
                }
                break;
            case '"':
                switch (current_state)
                {
                case START:
                    text.push_back(c);
                    current_state = String;
                    break;
                case String:
                    text.push_back(c);
                    tokens.push_back(finish_token());
                    text.clear();
                    break;
                default:
                    text.push_back(c);
                }
                break;
            case '(':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = LPAREN;
                break;
            case ')':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = RPAREN;
                break;
            case '{':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = LBRACE;
                break;
            case '}':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = RBRACE;
                break;
            case '[':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = LBRACKET;
                break;
            case ']':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = RBRACKET;
                break;
            case ',':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = COMMA;
                break;
            case ':':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = COLON;
                break;
            case ';':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = SEMICOLON;
                break;
            case '+':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = PLUS;
                break;
            case '-':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = MINUS;
                break;
            case '*':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = STAR;
                break;
            case '&':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = AMP;
                break;
            case '|':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = BAR;
                break;
            case '~':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = TILDE;
                break;
            case '=':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    text.push_back('=');
                    current_state = EQEQ;
                }
                else
                    current_state = EQ;
                break;
            case '>':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    text.push_back('=');
                    current_state = GTEQ;
                }
                else
                    current_state = GT;
                break;
            case '<':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    text.push_back('=');
                    current_state = LTEQ;
                }
                else
                    current_state = LT;
                break;
            case '!':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    text.push_back('=');
                    current_state = BANGEQ;
                }
                else
                    current_state = BANG;
                break;
            case '^':
                if (!text.empty())
                    tokens.push_back(finish_token());
                text.push_back(c);
                current_state = CARET;
                break;
            case '\r':
                start = 0;
                [[fallthrough]];
            case '\n':
                ++line;
                [[fallthrough]];
            case ' ':
            case '\t':
                if (!text.empty())
                    tokens.push_back(finish_token());
                ++start;
                break;
            case EOF:
                if (!text.empty())
                    tokens.push_back(finish_token());
                tokens.push_back(std::make_unique<token>(EoF, line, start, start));
                break;
            default:
                if (current_state == START)
                    current_state = ID;
                text.push_back(c);
            }
        }

        return tokens;
    }

    std::unique_ptr<token> lexer::finish_token()
    {
        std::unique_ptr<token> tok;
        auto end = start + text.size() - 1;
        switch (current_state)
        {
        case Bool:
            tok = std::make_unique<bool_token>(text == "true", line, start, end);
            break;
        case Int:
            tok = std::make_unique<int_token>(std::stoll(text), line, start, end);
            break;
        case Real:
        {
            size_t dot_pos = text.find('.');
            std::string intgr = text.substr(0, dot_pos);
            std::string dec = text.substr(dot_pos + 1);
            tok = std::make_unique<real_token>(utils::rational(static_cast<INT_TYPE>(std::stol(intgr + dec)), static_cast<INT_TYPE>(std::pow(10, dec.size()))), line, start, end);
            break;
        }
        case String:
            tok = std::make_unique<string_token>(std::move(text), line, start, end);
            break;
        case ID:
            tok = std::make_unique<id_token>(std::move(text), line, start, end);
            break;
        default:
            tok = std::make_unique<token>(current_state, line, start, end);
        }
        start = end + 1;
        current_state = START;
        text.clear();
        return tok;
    }
} // namespace riddle
