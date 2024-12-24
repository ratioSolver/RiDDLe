#include "parser.hpp"
#include <stdexcept>
#include <cassert>

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
                    pos -= 3;
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

        if (!match(SEMICOLON))
            error("Expected `;` after class declaration");

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
        std::vector<id_token> rt;
        std::vector<std::pair<std::vector<id_token>, id_token>> params;
        std::vector<std::unique_ptr<statement>> stmts;

        switch (tokens.at(pos++)->sym)
        {
        case VOID:
            break;
        case BOOL:
            rt.emplace_back(std::string(bool_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case INT:
            rt.emplace_back(std::string(int_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case REAL:
            rt.emplace_back(std::string(real_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case TIME:
            rt.emplace_back(std::string(time_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case STRING:
            rt.emplace_back(std::string(string_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        case ID:
        {
            size_t c_pos = pos++;
            while (match(DOT))
                if (!match(ID))
                    error("Expected identifier after `.`");
            if (!match(ID))
                error("Expected identifier after `.`");
            rt.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            pos = c_pos;
            break;
        }
        default:
            error("Expected type");
        }

        if (!match(ID))
            error("Expected identifier");

        id_token name = static_cast<const id_token &>(*tokens.at(pos - 1));

        if (!match(LPAREN))
            error("Expected `(` after method name");

        if (!match(RPAREN))
        {
            do
            {
                std::vector<id_token> tp;
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

                if (!match(ID))
                    error("Expected identifier");
                params.emplace_back(std::move(tp), static_cast<const id_token &>(*tokens.at(pos - 1)));
            } while (match(COMMA));

            if (!match(RPAREN))
                error("Expected `)` after parameters");
        }

        if (!match(LBRACE))
            error("Expected `{` after method declaration");

        while (!match(RBRACE))
            stmts.emplace_back(parse_statement());

        return std::make_unique<method_declaration>(std::move(rt), std::move(name), std::move(params), std::move(stmts));
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
        switch (tokens.at(pos++)->sym)
        {
        case BOOL:
        { // a local field having a bool type..
            std::vector<id_token> field_type;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

            field_type.emplace_back(std::string(bool_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            do
            {
                if (!match(ID))
                    error("Expected identifier");
                id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case INT:
        { // a local field having a int type..
            std::vector<id_token> field_type;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

            field_type.emplace_back(std::string(int_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            do
            {
                if (!match(ID))
                    error("Expected identifier");
                id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case REAL:
        { // a local field having a real type..
            std::vector<id_token> field_type;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

            field_type.emplace_back(std::string(real_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            do
            {
                if (!match(ID))
                    error("Expected identifier");
                id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case TIME:
        { // a local field having a time type..
            std::vector<id_token> field_type;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

            field_type.emplace_back(std::string(time_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            do
            {
                if (!match(ID))
                    error("Expected identifier");
                id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case STRING:
        { // a local field having a string type..
            std::vector<id_token> field_type;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

            field_type.emplace_back(std::string(string_kw), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            do
            {
                if (!match(ID))
                    error("Expected identifier");
                id_token id = static_cast<const id_token &>(*tokens.at(pos - 1));
                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        }
    }

    std::unique_ptr<expression> parser::parse_expression(std::size_t precedence)
    {
        std::unique_ptr<expression> expr;
        switch (tokens.at(pos++)->sym)
        {
        case Bool:
            expr = std::make_unique<bool_expression>(bool_token(static_cast<const bool_token &>(*tokens.at(pos - 1)).value, tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            break;
        case Int:
            expr = std::make_unique<int_expression>(int_token(static_cast<const int_token &>(*tokens.at(pos - 1)).value, tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            break;
        case Real:
            expr = std::make_unique<real_expression>(real_token(utils::rational(static_cast<const real_token &>(*tokens.at(pos - 1)).value), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            break;
        case String:
            expr = std::make_unique<string_expression>(string_token(std::string(static_cast<const string_token &>(*tokens.at(pos - 1)).value), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            break;
        case LBRACKET:
            switch (tokens.at(pos++)->sym)
            {
            case INT:
                if (!match(COMMA))
                    error("Expected `,` after `int`");
                if (!match(INT))
                    error("Expected `int` after `,`");
                if (!match(RBRACKET))
                    error("Expected `]` after `int`");
                expr = std::make_unique<bounded_int_expression>(int_token(static_cast<const int_token &>(*tokens.at(pos - 4)).value, tokens.at(pos - 4)->line, tokens.at(pos - 4)->start_pos, tokens.at(pos - 4)->end_pos), int_token(static_cast<const int_token &>(*tokens.at(pos - 2)).value, tokens.at(pos - 2)->line, tokens.at(pos - 2)->start_pos, tokens.at(pos - 2)->end_pos));
                break;
            case REAL:
                if (!match(COMMA))
                    error("Expected `,` after `real`");
                if (!match(REAL))
                    error("Expected `real` after `,`");
                if (!match(RBRACKET))
                    error("Expected `]` after `real`");
                expr = std::make_unique<bounded_real_expression>(real_token(utils::rational(static_cast<const real_token &>(*tokens.at(pos - 4)).value), tokens.at(pos - 4)->line, tokens.at(pos - 4)->start_pos, tokens.at(pos - 4)->end_pos), real_token(utils::rational(static_cast<const real_token &>(*tokens.at(pos - 2)).value), tokens.at(pos - 2)->line, tokens.at(pos - 2)->start_pos, tokens.at(pos - 2)->end_pos));
                break;
            default:
                error("Expected `int` or `real` after `[`");
            }
            break;
        case QUESTION:
            if (!match(LBRACKET))
                error("Expected `[` after `?`");
            switch (tokens.at(pos++)->sym)
            {
            case INT:
                if (!match(COMMA))
                    error("Expected `,` after `int`");
                if (!match(INT))
                    error("Expected `int` after `,`");
                if (!match(RBRACKET))
                    error("Expected `]` after `int`");
                expr = std::make_unique<uncertain_int_expression>(int_token(static_cast<const int_token &>(*tokens.at(pos - 4)).value, tokens.at(pos - 4)->line, tokens.at(pos - 4)->start_pos, tokens.at(pos - 4)->end_pos), int_token(static_cast<const int_token &>(*tokens.at(pos - 2)).value, tokens.at(pos - 2)->line, tokens.at(pos - 2)->start_pos, tokens.at(pos - 2)->end_pos));
                break;
            case REAL:
                if (!match(COMMA))
                    error("Expected `,` after `real`");
                if (!match(REAL))
                    error("Expected `real` after `,`");
                if (!match(RBRACKET))
                    error("Expected `]` after `real`");
                expr = std::make_unique<uncertain_real_expression>(real_token(utils::rational(static_cast<const real_token &>(*tokens.at(pos - 4)).value), tokens.at(pos - 4)->line, tokens.at(pos - 4)->start_pos, tokens.at(pos - 4)->end_pos), real_token(utils::rational(static_cast<const real_token &>(*tokens.at(pos - 2)).value), tokens.at(pos - 2)->line, tokens.at(pos - 2)->start_pos, tokens.at(pos - 2)->end_pos));
                break;
            default:
                error("Expected `int` or `real` after `[`");
            }
            break;
        case MINUS:
            expr = std::make_unique<minus_expression>(parse_expression());
            break;
        case BANG:
            expr = std::make_unique<not_expression>(parse_expression());
            break;
        case ID:
        {
            std::vector<id_token> object_id;
            object_id.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            while (match(DOT))
            {
                if (!match(ID))
                    error("Expected identifier after `.`");
                object_id.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            }
            if (match(LPAREN))
            { // call expression..
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
                expr = std::make_unique<call_expression>(std::move(object_id), id_token(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos), std::move(args));
            }
            else // id expression..
                expr = std::make_unique<id_expression>(std::move(object_id));
            break;
        }
        case NEW:
        {
            std::vector<id_token> type_id;
            if (!match(ID))
                error("Expected identifier after `new`");
            type_id.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            while (match(DOT))
            {
                if (!match(ID))
                    error("Expected identifier after `.`");
                type_id.emplace_back(static_cast<const id_token &>(*tokens.at(pos - 1)));
            }
            if (!match(LPAREN))
                error("Expected `(` after type");
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
            expr = std::make_unique<constructor_expression>(std::move(type_id), std::move(args));
            break;
        }
        case LPAREN:
            expr = parse_expression();
            if (!match(RPAREN))
                error("Expected `)` after expression");
            break;
        default:
            error("Unexpected token");
        }

        while (
            ((tokens.at(pos)->sym == EQEQ || tokens.at(pos)->sym == BANGEQ) && 0 >= precedence) ||
            ((tokens.at(pos)->sym == IMPLICATION || tokens.at(pos)->sym == BAR || tokens.at(pos)->sym == AMP || tokens.at(pos)->sym == CARET) && 1 >= precedence) ||
            ((tokens.at(pos)->sym == LT || tokens.at(pos)->sym == LTEQ || tokens.at(pos)->sym == GTEQ || tokens.at(pos)->sym == GT) && 2 >= precedence) ||
            ((tokens.at(pos)->sym == PLUS || tokens.at(pos)->sym == MINUS) && 3 >= precedence) ||
            ((tokens.at(pos)->sym == STAR || tokens.at(pos)->sym == SLASH) && 4 >= precedence))
            switch (tokens.at(pos++)->sym)
            {
            case EQEQ:
                assert(0 >= precedence);
                expr = std::make_unique<eq_expression>(std::move(expr), parse_expression(1));
                break;
            case BANGEQ:
                assert(0 >= precedence);
                expr = std::make_unique<not_expression>(std::make_unique<eq_expression>(std::move(expr), parse_expression(1)));
                break;
            case IMPLICATION:
            {
                assert(1 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::make_unique<not_expression>(std::move(expr)));
                xprs.emplace_back(parse_expression(2));
                expr = std::make_unique<or_expression>(std::move(xprs));
                break;
            }
            case BAR:
            {
                assert(1 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));
                while (match(BAR))
                    xprs.emplace_back(parse_expression(2));
                expr = std::make_unique<or_expression>(std::move(xprs));
                break;
            }
            case AMP:
            {
                assert(1 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));
                while (match(AMP))
                    xprs.emplace_back(parse_expression(2));
                expr = std::make_unique<and_expression>(std::move(xprs));
                break;
            }
            case CARET:
            {
                assert(1 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));
                while (match(CARET))
                    xprs.emplace_back(parse_expression(2));
                expr = std::make_unique<xor_expression>(std::move(xprs));
                break;
            }
            case LT:
                assert(2 >= precedence);
                expr = std::make_unique<lt_expression>(std::move(expr), parse_expression(3));
                break;
            case LTEQ:
                assert(2 >= precedence);
                expr = std::make_unique<le_expression>(std::move(expr), parse_expression(3));
                break;
            case GTEQ:
                assert(2 >= precedence);
                expr = std::make_unique<ge_expression>(std::move(expr), parse_expression(3));
                break;
            case GT:
                assert(2 >= precedence);
                expr = std::make_unique<gt_expression>(std::move(expr), parse_expression(3));
                break;
            case PLUS:
            {
                assert(3 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));
                while (match(PLUS))
                    xprs.emplace_back(parse_expression(4));
                expr = std::make_unique<sum_expression>(std::move(xprs));
                break;
            }
            case STAR:
            {
                assert(4 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));
                while (match(STAR))
                    xprs.emplace_back(parse_expression(5));
                expr = std::make_unique<product_expression>(std::move(xprs));
                break;
            }
            case SLASH:
                assert(4 >= precedence);
                expr = std::make_unique<divide_expression>(std::move(expr), parse_expression(5));
                break;
            default:
                error("Unexpected token");
            }

        return expr;
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
