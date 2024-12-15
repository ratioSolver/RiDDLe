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
                }
                break;
            case 'b':
                if (current_state == START)
                    current_state = BOOL;
                text.push_back(c);
                break;
            case 'c':
                if (current_state == START)
                    current_state = CLASS;
                text.push_back(c);
                break;
            case 'e':
                if (current_state == START)
                    current_state = ENUM;
                text.push_back(c);
                break;
            case 'f':
                if (current_state == START)
                    current_state = FOR;
                text.push_back(c);
                break;
            case 'g':
                if (current_state == START)
                    current_state = GOAL;
                text.push_back(c);
                break;
            case 'h':
                if (current_state == TIME && text == "t") // th
                    current_state = THIS;
                text.push_back(c);
                break;
            case 'i':
                if (current_state == START)
                    current_state = INT;
                text.push_back(c);
                break;
            case 'l':
                if (current_state == FACT && text == "fa") // fal
                    current_state = Bool;
                text.push_back(c);
                break;
            case 'n':
                if (current_state == START)
                    current_state = NEW;
                text.push_back(c);
                break;
            case 'o':
                if (current_state == START)
                    current_state = OR;
                text.push_back(c);
                break;
            case 'p':
                if (current_state == START)
                    current_state = PREDICATE;
                else
                    text.push_back(c);
                break;
            case 'r':
                if (current_state == START)
                    current_state = REAL;
                else if (current_state == TIME && text == "t") // tr
                    current_state = Bool;
                text.push_back(c);
                break;
            case 's':
                if (current_state == START)
                    current_state = STRING;
                text.push_back(c);
                break;
            case 't':
                if (current_state == START)
                    current_state = TIME;
                else if (current_state == REAL && text == "re") // ret
                    current_state = RETURN;
                text.push_back(c);
                break;
            case 'v':
                if (current_state == START)
                    current_state = VOID;
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
                    current_state = Int;
                else if (current_state != ID && current_state != Int && current_state != Real)
                    tokens.push_back(finish_token());
                text.push_back(c);
                break;
            case '.':
                if ((current_state == START && std::isdigit(is.peek())) || current_state == Int)
                {
                    current_state = Real;
                    text.push_back(c);
                }
                else
                {
                    if (!text.empty())
                        tokens.push_back(finish_token());
                    current_state = DOT;
                    text.push_back(c);
                    tokens.push_back(finish_token());
                }
                break;
            case '"':
                switch (current_state)
                {
                case START:
                    current_state = String;
                    text.push_back(c);
                    break;
                case String:
                    text.push_back(c);
                    tokens.push_back(finish_token());
                    break;
                default:
                    throw std::runtime_error("Unexpected character: " + std::string(1, c));
                }
                break;
            case '(':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = LPAREN;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case ')':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = RPAREN;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '{':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = LBRACE;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '}':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = RBRACE;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '[':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = LBRACKET;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case ']':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = RBRACKET;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case ',':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = COMMA;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case ':':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = COLON;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case ';':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = SEMICOLON;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '+':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = PLUS;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '-':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = MINUS;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '*':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = STAR;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '&':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = AMP;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '|':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = BAR;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '~':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = TILDE;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '=':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = EQ;
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    current_state = EQEQ;
                    text.push_back('=');
                }
                tokens.push_back(finish_token());
                break;
            case '>':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = GT;
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    current_state = GTEQ;
                    text.push_back('=');
                }
                tokens.push_back(finish_token());
                break;
            case '<':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = LT;
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    current_state = LTEQ;
                    text.push_back('=');
                }
                tokens.push_back(finish_token());
                break;
            case '!':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = BANG;
                text.push_back(c);
                if (is.peek() == '=')
                {
                    is.ignore();
                    current_state = BANGEQ;
                    text.push_back('=');
                }
                tokens.push_back(finish_token());
                break;
            case '^':
                if (!text.empty())
                    tokens.push_back(finish_token());
                current_state = CARET;
                text.push_back(c);
                tokens.push_back(finish_token());
                break;
            case '\r':
                if (current_state == String)
                    text.push_back(c);
                else
                {
                    if (!text.empty())
                        tokens.push_back(finish_token());
                    start = 0;
                }
                break;
            case '\n':
                if (current_state == String)
                    text.push_back(c);
                else
                {
                    if (!text.empty())
                        tokens.push_back(finish_token());
                    start = 0;
                    ++line;
                }
                break;
            case ' ':
            case '\t':
                if (current_state == String)
                    text.push_back(c);
                else
                {
                    if (!text.empty())
                        tokens.push_back(finish_token());
                    ++start;
                }
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
            tok = std::make_unique<string_token>(std::move(text.substr(1, text.size() - 2)), line, start, end);
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
