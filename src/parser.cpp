#include "parser.hpp"
#include <stdexcept>

namespace riddle
{
    std::unique_ptr<compilation_unit> parser::parse_compilation_unit()
    {
        std::vector<std::unique_ptr<type_declaration>> types;           // the type declarations..
        std::vector<std::unique_ptr<predicate_declaration>> predicates; // the predicate declarations..
        std::vector<std::unique_ptr<method_declaration>> methods;       // the method declarations..
        std::vector<std::unique_ptr<statement>> statements;             // the statements..

        while (!match(symbol::EoF))
        {
            switch (tokens.at(pos)->sym)
            {
            case symbol::ENUM:
                types.push_back(parse_enum_declaration());
                break;
            case symbol::CLASS:
                types.push_back(parse_class_declaration());
                break;
            case symbol::PREDICATE:
                predicates.push_back(parse_predicate_declaration());
                break;
            case symbol::VOID:
                methods.push_back(parse_method_declaration());
                break;
            case BOOL:
            case INT:
            case REAL:
            case TIME:
            case STRING:
            case LBRACE:
            case BANG:
            case FACT:
            case GOAL:
            case Bool:
            case Int:
            case Real:
            {
                size_t c_pos = pos;
                next();
                if (match(ID) && match(LPAREN))
                {
                    backtrack(c_pos);
                    methods.push_back(parse_method_declaration());
                }
                else
                {
                    backtrack(c_pos);
                    statements.push_back(parse_statement());
                }
                break;
            }
            case ID:
            {
                size_t c_pos = pos;
                next();
                while (match(DOT))
                    if (!match(ID))
                        error("Expected identifier after '.'");
                if (match(ID) && match(LPAREN))
                {
                    backtrack(c_pos);
                    methods.push_back(parse_method_declaration());
                }
                else
                {
                    backtrack(c_pos);
                    statements.push_back(parse_statement());
                }
                break;
            }
            default:
                error("Unexpected token");
            }
        }
        return std::make_unique<compilation_unit>(std::move(types), std::move(methods), std::move(predicates), std::move(statements));
    }

    std::unique_ptr<enum_declaration> parser::parse_enum_declaration()
    {
        if (!match(symbol::ENUM))
            error("Expected 'enum' keyword");
        if (!match(ID))
            error("Expected identifier after 'enum' keyword");

        auto id = static_cast<const id_token &>(*tokens.at(pos - 1));
        std::vector<string_token> values;             // the values of the enum..
        std::vector<std::vector<id_token>> enum_refs; // the enum references..

        do
        {
            switch (next().sym)
            {
            case LBRACE:
                while (!match(RBRACE))
                {
                    if (!match(String))
                        error("Expected string literal");
                    values.push_back(static_cast<const string_token &>(*tokens.at(pos - 1)));
                    if (!match(COMMA) && !match(RBRACE))
                        error("Expected ',' or '}' after string literal");
                }
                break;
            case ID:
            {
                std::vector<id_token> refs;
                refs.push_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
                while (match(DOT))
                {
                    if (!match(ID))
                        error("Expected identifier after '.'");
                    refs.push_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
                }
                enum_refs.push_back(std::move(refs));
                break;
            }
            default:
                error("Expected '{' or '|'");
            }
        } while (match(BAR));

        if (!match(SEMICOLON))
            error("Expected ';' after enum declaration");

        return std::make_unique<enum_declaration>(std::move(id), std::move(values), std::move(enum_refs));
    }

    bool parser::match(const symbol &sym)
    {
        if (tokens.at(pos)->sym == sym)
        {
            next();
            return true;
        }
        return false;
    }

    const token &parser::next()
    {
        while (pos >= tokens.size())
            tokens.push_back(lex.next_token());
        return *tokens[pos++];
    }

    void parser::backtrack(const size_t &p) noexcept { pos = p; }

    void parser::error(std::string &&err) { throw std::invalid_argument("[" + std::to_string(tokens.at(pos)->line) + ":" + std::to_string(tokens.at(pos)->start_pos) + "] " + std::move(err)); }
} // namespace riddle
