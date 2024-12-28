#include "parser.hpp"
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
                pos++;
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
                    pos = c_pos;
                    methods.emplace_back(parse_method_declaration());
                }
                else
                { // statement..
                    pos = c_pos;
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

        id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
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
                        values.emplace_back(std::string(static_cast<const string_token &>(*tokens.at(pos - 1)).value), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                    } while (match(COMMA));
                    if (!match(RBRACE))
                        error("Expected `}` after string literals");
                }
                break;
            case ID:
            {
                std::vector<id_token> refs;
                refs.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                while (match(DOT))
                {
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    refs.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
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

        id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

        if (match(COLON))
            do
            {
                std::vector<id_token> base_class;
                do
                {
                    if (!match(ID))
                        error("Expected identifier after `:`");
                    base_class.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
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
                    size_t c_pos = pos;
                    while (match(DOT))
                        if (!match(ID))
                            error("Expected identifier after `.`");
                    if (match(LPAREN))
                    { // method declaration..
                        pos = c_pos;
                        methods.emplace_back(parse_method_declaration());
                    }
                    else
                    { // field declaration..
                        pos = c_pos;
                        fields.emplace_back(parse_field_declaration());
                    }
                }
                break;
            }
            case ENUM:
                pos--;
                types.emplace_back(parse_enum_declaration());
                break;
            case CLASS:
                pos--;
                types.emplace_back(parse_class_declaration());
                break;
            case PREDICATE:
                pos--;
                predicates.emplace_back(parse_predicate_declaration());
                break;
            case VOID:
                pos--;
                methods.emplace_back(parse_method_declaration());
                break;
            default:
                error("Unexpected token");
            }

        if (!match(SEMICOLON))
            error("Expected `;` after class declaration");

        if (constructors.empty()) // default constructor..
            constructors.emplace_back(std::make_unique<constructor_declaration>(std::vector<std::pair<std::vector<id_token>, id_token>>(), std::vector<std::pair<id_token, std::vector<std::unique_ptr<expression>>>>(), std::vector<std::unique_ptr<statement>>()));

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
            while (match(DOT))
                if (!match(ID))
                    error("Expected identifier after `.`");
            if (!match(ID))
                error("Expected identifier after `.`");
            tp.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        }
        default:
            error("Expected type");
        }

        do
        {
            if (!match(ID))
                error("Expected identifier");

            id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

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
                std::vector<id_token> f_tp;
                f_tp.reserve(tp.size());
                for (const auto &t : tp)
                    f_tp.emplace_back(std::string(t.id), t.line, t.start_pos, t.end_pos);
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
            while (match(DOT))
                if (!match(ID))
                    error("Expected identifier after `.`");
            if (!match(ID))
                error("Expected identifier after `.`");
            rt.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            break;
        }
        default:
            error("Expected type");
        }

        if (!match(ID)) // method name..
            error("Expected identifier");

        id_token name(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

        if (!match(LPAREN))
            error("Expected `(` after method name");

        if (!match(RPAREN))
        { // we parse the parameters..
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
                    while (match(DOT))
                        if (!match(ID))
                            error("Expected identifier after `.`");
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    tp.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                    break;
                }
                default:
                    error("Expected type");
                }

                if (!match(ID))
                    error("Expected identifier");
                params.emplace_back(std::move(tp), id_token(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
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
        std::vector<std::pair<std::vector<id_token>, id_token>> params;
        std::vector<std::pair<id_token, std::vector<std::unique_ptr<expression>>>> inits;
        std::vector<std::unique_ptr<statement>> stmts;

        if (!match(ID)) // constructor name..
            error("Expected identifier");

        if (!match(LPAREN))
            error("Expected `(` after constructor name");

        if (!match(RPAREN))
        { // we parse the parameters..
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
                    while (match(DOT))
                        if (!match(ID))
                            error("Expected identifier after `.`");
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    tp.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                    break;
                }
                default:
                    error("Expected type");
                }

                if (!match(ID))
                    error("Expected identifier");
                params.emplace_back(std::move(tp), id_token(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            } while (match(COMMA));

            if (!match(RPAREN))
                error("Expected `)` after parameters");
        }

        if (match(COLON))
            do // we parse the initializations..
            {
                if (!match(ID))
                    error("Expected identifier");

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

                if (!match(LPAREN))
                    error("Expected `(` after identifier");

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

                inits.emplace_back(std::move(id), std::move(args));
            } while (match(COMMA));

        if (!match(LBRACE))
            error("Expected `{` after constructor declaration");

        while (!match(RBRACE))
            stmts.emplace_back(parse_statement());

        return std::make_unique<constructor_declaration>(std::move(params), std::move(inits), std::move(stmts));
    }

    std::unique_ptr<predicate_declaration> parser::parse_predicate_declaration()
    {
        if (!match(PREDICATE))
            error("Expected `predicate` keyword");

        if (!match(ID)) // predicate name..
            error("Expected identifier after `predicate` keyword");

        id_token name(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
        std::vector<std::pair<std::vector<id_token>, id_token>> params;
        std::vector<std::vector<id_token>> base_predicates;
        std::vector<std::unique_ptr<statement>> body;

        if (!match(LPAREN))
            error("Expected `(` after predicate name");

        if (!match(RPAREN))
        { // we parse the parameters..
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
                    while (match(DOT))
                        if (!match(ID))
                            error("Expected identifier after `.`");
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    tp.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                    break;
                }
                default:
                    error("Expected type");
                }

                if (!match(ID))
                    error("Expected identifier");
                params.emplace_back(std::move(tp), id_token(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos));
            } while (match(COMMA));

            if (!match(RPAREN))
                error("Expected `)` after parameters");
        }

        if (match(COLON))
            do // we parse the base predicates..
            {
                std::vector<id_token> base_predicate;
                do
                {
                    if (!match(ID))
                        error("Expected identifier");
                    base_predicate.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                } while (match(DOT));
                base_predicates.emplace_back(std::move(base_predicate));
            } while (match(COMMA));

        if (!match(LBRACE))
            error("Expected `{` after predicate declaration");

        while (!match(RBRACE))
            body.emplace_back(parse_statement());

        return std::make_unique<predicate_declaration>(std::move(name), std::move(params), std::move(base_predicates), std::move(body));
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

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

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

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

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

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

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

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

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

                id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

                if (match(EQ))
                    fields.emplace_back(std::move(id), parse_expression());
                else
                    fields.emplace_back(std::move(id), nullptr);
            } while (match(COMMA));

            if (!match(SEMICOLON))
                error("Expected `;` after local field declaration");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case ID:
        { // either a local field, an assignment or an expression..
            size_t c_pos = pos;
            while (match(DOT))
                if (!match(ID))
                    error("Expected identifier after `.`");

            switch (tokens.at(pos)->sym)
            {
            case ID: // a local field..
            {
                pos = c_pos - 1;
                std::vector<id_token> field_type;
                std::vector<std::pair<id_token, std::unique_ptr<expression>>> fields;

                if (!match(ID))
                    error("Expected identifier");
                field_type.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

                while (match(DOT))
                    if (match(ID))
                        field_type.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                    else
                        error("Expected identifier after `.`");

                do
                {
                    if (!match(ID))
                        error("Expected identifier");

                    id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

                    if (match(EQ))
                        fields.emplace_back(std::move(id), parse_expression());
                    else
                        fields.emplace_back(std::move(id), nullptr);
                } while (match(COMMA));

                if (!match(SEMICOLON))
                    error("Expected `;` after local field declaration");

                return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
            }
            case EQ: // an assignment..
            {
                pos = c_pos - 1;
                std::vector<id_token> object_id;
                object_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(c_pos)).id), tokens.at(c_pos)->line, tokens.at(c_pos)->start_pos, tokens.at(c_pos)->end_pos);
                while (match(DOT))
                {
                    if (!match(ID))
                        error("Expected identifier after `.`");
                    object_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
                }

                id_token field_id = std::move(object_id.back());
                object_id.pop_back();

                if (!match(EQ))
                    error("Expected `=` after object identifier");

                auto xpr = parse_expression();

                if (!match(SEMICOLON))
                    error("Expected `;` after assignment");

                return std::make_unique<assignment_statement>(std::move(object_id), std::move(field_id), std::move(xpr));
            }
            default: // an expression..
                pos = c_pos - 1;
                auto xpr = parse_expression();

                if (!match(SEMICOLON))
                    error("Expected `;` after expression");

                return std::make_unique<expression_statement>(std::move(xpr));
            }
        }
        case LBRACE:
        { // a conjunction or a disjunction..
            std::vector<std::unique_ptr<conjunction_statement>> conjuncts;
            pos--;
            do // conjunction..
            {
                std::vector<std::unique_ptr<statement>> stmts;
                if (!match(LBRACE))
                    error("Expected `{` after `conjunction`");
                while (!match(RBRACE))
                    stmts.emplace_back(parse_statement());

                if (match(LBRACKET))
                { // a priced conjunction..
                    conjuncts.emplace_back(std::make_unique<conjunction_statement>(std::move(stmts), parse_expression()));
                    if (!match(RBRACKET))
                        error("Expected `]` after priced conjunction");
                }
                else // a simple conjunction..
                    conjuncts.emplace_back(std::make_unique<conjunction_statement>(std::move(stmts)));
            } while (match(OR));

            if (conjuncts.size() == 1) // a simple conjunction..
                return std::move(conjuncts.front());
            else // a disjunction..
                return std::make_unique<disjunction_statement>(std::move(conjuncts));
        }
        case FOR:
        { // a for loop..
            if (!match(LPAREN))
                error("Expected `(` after `for`");
            std::vector<id_token> enum_type;
            std::vector<std::unique_ptr<statement>> stmts;
            do // the enum type..
            {
                if (!match(ID))
                    error("Expected identifier");
                enum_type.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            } while (match(DOT));

            if (!match(ID))
                error("Expected identifier");

            id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            if (!match(RPAREN))
                error("Expected `)` after for loop");

            if (!match(LBRACE))
                error("Expected `{` after for loop");

            while (!match(RBRACE))
                stmts.emplace_back(parse_statement());

            return std::make_unique<for_all_statement>(std::move(enum_type), std::move(id), std::move(stmts));
        }
        case RETURN:
        { // a return statement..
            auto xpr = parse_expression();
            if (!match(SEMICOLON))
                error("Expected `;` after return statement");
            return std::make_unique<return_statement>(std::move(xpr));
        }
        case FACT:
        case GOAL:
        { // a fact or a goal..
            bool is_fact = tokens.at(pos - 1)->sym == FACT;
            std::vector<id_token> tau;
            std::vector<std::pair<id_token, std::unique_ptr<expression>>> args;

            if (!match(ID))
                error("Expected identifier");

            id_token name(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

            if (!match(EQ))
                error("Expected `=` after atom name");

            if (!match(NEW))
                error("Expected `new` after `=`");

            do // the formula scope..
            {
                if (!match(ID))
                    error("Expected identifier");
                tau.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            } while (match(DOT));

            auto predicate_name = std::move(tau.back());
            tau.pop_back();

            if (!match(LPAREN))
                error("Expected `(` after predicate name");

            if (!match(RPAREN))
            { // the arguments of the atom..
                do
                {
                    if (!match(ID))
                        error("Expected identifier");

                    id_token id(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);

                    if (match(COLON))
                        args.emplace_back(std::move(id), parse_expression());
                    else
                        args.emplace_back(std::move(id), nullptr);
                } while (match(COMMA));

                if (!match(RPAREN))
                    error("Expected `)` after arguments");
            }

            if (!match(SEMICOLON))
                error("Expected `;` after fact or goal");

            return std::make_unique<formula_statement>(is_fact, std::move(name), std::move(tau), std::move(predicate_name), std::move(args));
        }
        default:
            pos--;
            auto xpr = parse_expression();

            if (!match(SEMICOLON))
                error("Expected `;` after expression");

            return std::make_unique<expression_statement>(std::move(xpr));
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
            object_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            while (match(DOT))
            {
                if (!match(ID))
                    error("Expected identifier after `.`");
                object_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
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
            type_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
            while (match(DOT))
            {
                if (!match(ID))
                    error("Expected identifier after `.`");
                type_id.emplace_back(std::string(static_cast<const id_token &>(*tokens.at(pos - 1)).id), tokens.at(pos - 1)->line, tokens.at(pos - 1)->start_pos, tokens.at(pos - 1)->end_pos);
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
            switch (tokens.at(pos)->sym)
            {
            case EQEQ:
                assert(0 >= precedence);
                pos++; // we parse the `==` operator..
                expr = std::make_unique<eq_expression>(std::move(expr), parse_expression(1));
                break;
            case BANGEQ:
                assert(0 >= precedence);
                pos++; // we parse the `!=` operator..
                expr = std::make_unique<not_expression>(std::make_unique<eq_expression>(std::move(expr), parse_expression(1)));
                break;
            case IMPLICATION:
            {
                assert(1 >= precedence);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::make_unique<not_expression>(std::move(expr)));
                pos++; // we parse the `=>` operator..
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
                pos++; // we parse the `<` operator..
                expr = std::make_unique<lt_expression>(std::move(expr), parse_expression(3));
                break;
            case LTEQ:
                assert(2 >= precedence);
                pos++; // we parse the `<=` operator..
                expr = std::make_unique<le_expression>(std::move(expr), parse_expression(3));
                break;
            case GTEQ:
                assert(2 >= precedence);
                pos++; // we parse the `>=` operator..
                expr = std::make_unique<ge_expression>(std::move(expr), parse_expression(3));
                break;
            case GT:
                assert(2 >= precedence);
                pos++; // we parse the `>` operator..
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
                pos++; // we parse the `/` operator..
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
