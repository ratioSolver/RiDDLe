#include "parser.hpp"
#include <stdexcept>

namespace riddle
{
    parser::parser(std::istream &is) : lex(), tokens(lex.parse(is)), pos(0) {}

    std::unique_ptr<compilation_unit> parser::parse_compilation_unit()
    {
        std::vector<std::unique_ptr<type_declaration>> types;           // the type declarations..
        std::vector<std::unique_ptr<predicate_declaration>> predicates; // the predicate declarations..
        std::vector<std::unique_ptr<method_declaration>> methods;       // the method declarations..
        std::vector<std::unique_ptr<statement>> statements;             // the statements..

        while (!match(EoF))
        {
            switch (tokens.at(pos)->sym)
            {
            case ENUM:
                types.emplace_back(parse_enum_declaration());
                break;
            case CLASS:
                types.emplace_back(parse_class_declaration());
                break;
            case PREDICATE:
                predicates.emplace_back(parse_predicate_declaration());
                break;
            case VOID:
                methods.emplace_back(parse_method_declaration());
                break;
            case BOOL:
            case INT:
            case REAL:
            case TIME:
            case STRING:
            {
                if (!match(ID))
                    error("Expected identifier");

                if (match(LPAREN))
                { // method declaration..
                    pos -= 2;
                    methods.emplace_back(parse_method_declaration());
                }
                else
                { // statement..
                    pos -= 2;
                    statements.emplace_back(parse_statement());
                }

                break;
            }
            case ID:
            {
                size_t c_pos = pos++;
                while (match(DOT))
                    if (!match(ID))
                        error("Expected identifier after `.`");
                if (match(LPAREN))
                { // method declaration..
                    pos = c_pos - 1;
                    methods.emplace_back(parse_method_declaration());
                }
                else
                { // statement..
                    pos = c_pos - 1;
                    statements.emplace_back(parse_statement());
                }
                break;
            }
            case LBRACE:
            case BANG:
            case FACT:
            case GOAL:
            case Bool:
            case Int:
            case Real:
            case String:
            { // statement..
                pos--;
                statements.emplace_back(parse_statement());
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
        if (!match(ENUM))
            error("Expected `enum` keyword");
        if (!match(ID))
            error("Expected identifier after `enum` keyword");

        auto id = static_cast<const id_token &>(*tokens.at(pos - 1));
        std::vector<string_token> values;             // the values of the enum..
        std::vector<std::vector<id_token>> enum_refs; // the enum references..

        do
        {
            switch (tokens.at(pos++)->sym)
            {
            case LBRACE:
                if (!match(RBRACE))
                {
                    do
                    {
                        if (!match(String))
                            error("Expected string literal");
                        values.emplace_back(static_cast<const string_token &>(*tokens.at(pos - 1)));
                    } while (match(COMMA));
                    if (!match(RBRACE))
                        error("Expected `}` after string literals");
                }
                break;
            case ID:
            {
                std::vector<id_token> refs;
                refs.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
                while (match(DOT))
                {
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    refs.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
                }
                enum_refs.emplace_back(std::move(refs));
                break;
            }
            default:
                error("Expected `{` or `|`");
            }
        } while (match(BAR));

        if (!match(SEMICOLON))
            error("Expected `;` after enum declaration");

        return std::make_unique<enum_declaration>(std::move(id), std::move(values), std::move(enum_refs));
    }

    std::unique_ptr<class_declaration> parser::parse_class_declaration()
    {
        std::vector<std::vector<id_token>> base_classes;                    // the base classes of the class..
        std::vector<std::unique_ptr<field_declaration>> fields;             // the fields of the class..
        std::vector<std::unique_ptr<constructor_declaration>> constructors; // the constructors of the class..
        std::vector<std::unique_ptr<method_declaration>> methods;           // the methods of the class..
        std::vector<std::unique_ptr<predicate_declaration>> predicates;     // the predicates of the class..
        std::vector<std::unique_ptr<type_declaration>> types;               // the types of the class..

        if (!match(CLASS))
            error("Expected `class` keyword");

        if (!match(ID))
            error("Expected identifier after `class` keyword");

        auto id = static_cast<const id_token &>(*tokens.at(pos - 1));

        if (match(COLON))
            do
            {
                std::vector<id_token> base_class;
                do
                {
                    if (!match(ID))
                        error("Expected identifier after `:`");
                    base_class.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
                } while (match(DOT));
                base_classes.emplace_back(std::move(base_class));
            } while (match(COMMA));

        if (!match(LBRACE))
            error("Expected `{` after class declaration");

        while (!match(RBRACE))
            switch (tokens.at(pos++)->sym)
            {
            case BOOL:
            case INT:
            case REAL:
            case TIME:
            case STRING:
            {
                if (!match(ID))
                    error("Expected identifier");

                if (match(LPAREN))
                { // method declaration..
                    pos -= 2;
                    methods.emplace_back(parse_method_declaration());
                }
                else
                { // field declaration..
                    pos -= 2;
                    fields.emplace_back(parse_field_declaration());
                }

                break;
            }
            case ID:
            {
                if (match(LPAREN))
                { // constructor declaration..
                    pos -= 2;
                    constructors.emplace_back(parse_constructor_declaration());
                }
                else
                {
                    size_t c_pos = pos++;
                    while (match(DOT))
                        if (!match(ID))
                            error("Expected identifier after `.`");
                    if (match(LPAREN))
                    { // method declaration..
                        pos = c_pos - 1;
                        methods.emplace_back(parse_method_declaration());
                    }
                    else
                    { // field declaration..
                        pos = c_pos - 1;
                        fields.emplace_back(parse_field_declaration());
                    }
                }
                break;
            }
            case ENUM:
                types.emplace_back(parse_enum_declaration());
                break;
            case CLASS:
                types.emplace_back(parse_class_declaration());
                break;
            case PREDICATE:
                predicates.emplace_back(parse_predicate_declaration());
                break;
            case VOID:
                methods.emplace_back(parse_method_declaration());
                break;
            default:
                error("Unexpected token");
            }

        if (constructors.empty())
        {
            std::vector<std::pair<std::vector<id_token>, id_token>> params;
            std::vector<id_token> names;
            std::vector<std::vector<std::unique_ptr<expression>>> args;
            std::vector<std::unique_ptr<statement>> stmts;
            constructors.emplace_back(std::make_unique<constructor_declaration>(std::move(params), std::move(names), std::move(args), std::move(stmts)));
        }

        return std::make_unique<class_declaration>(std::move(id), std::move(base_classes), std::move(fields), std::move(constructors), std::move(methods), std::move(predicates), std::move(types));
    }

    std::unique_ptr<field_declaration> parser::parse_field_declaration()
    {
        std::vector<id_token> tp;
        std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

        switch (tokens.at(pos++)->sym)
        {
        case BOOL:
            tp.emplace_back(std::string(bool_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case INT:
            tp.emplace_back(std::string(int_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case REAL:
            tp.emplace_back(std::string(real_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case TIME:
            tp.emplace_back(std::string(time_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case STRING:
            tp.emplace_back(std::string(string_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case ID:
        {
            size_t c_pos = pos++;
            while (match(DOT))
                if (!match(ID))
                    error("Expected identifier after `.`");
            if (!match(ID))
                error("Expected identifier after `.`");
            tp.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            pos = c_pos;
            break;
        }
        default:
            error("Expected type");
        }

        do
        {
            if (!match(ID))
                error("Expected identifier");
            id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
            if (match(EQ))
                fields.emplace_back(std::move(id), parse_expression());
            else if (match(LPAREN))
            {
                std::vector<std::unique_ptr<expression>> args;
                if (!match(RPAREN))
                {
                    do
                    {
                        args.emplace_back(parse_expression());
                    } while (match(COMMA));
                    if (!match(RPAREN))
                        error("Expected `)` after arguments");
                }
                std::vector<id_token> f_tp = tp;
                fields.emplace_back(std::move(id), std::make_unique<constructor_expression>(std::move(f_tp), std::move(args)));
            }
            else
                fields.emplace_back(std::move(id), nullptr);
        } while (match(COMMA));

        if (!match(SEMICOLON))
            error("Expected `;` after field declaration");

        return std::make_unique<field_declaration>(std::move(tp), std::move(fields));
    }

    std::unique_ptr<method_declaration> parser::parse_method_declaration()
    {
        throw std::runtime_error("Not implemented");
    }

    std::unique_ptr<constructor_declaration> parser::parse_constructor_declaration()
    {
        throw std::runtime_error("Not implemented");
    }

    std::unique_ptr<predicate_declaration> parser::parse_predicate_declaration()
    {
        throw std::runtime_error("Not implemented");
    }

    std::unique_ptr<statement> parser::parse_statement()
    {
        throw std::runtime_error("Not implemented");
    }

    std::unique_ptr<expression> parser::parse_expression()
    {
        throw std::runtime_error("Not implemented");
    }

    bool parser::match(const symbol &sym)
    {
        if (tokens.at(pos)->sym == sym)
        {
            pos++;
            return true;
        }
        return false;
    }

    void parser::error(std::string &&err) { throw std::invalid_argument("[" + std::to_string(tokens.at(pos)->line) + ":" + std::to_string(tokens.at(pos)->start_pos) + "] " + std::move(err)); }
} // namespace riddle
