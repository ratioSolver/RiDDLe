#include <cassert>
#include "parser.hpp"

namespace riddle
{
    std::unique_ptr<compilation_unit> parser::parse()
    {
        tk = next_token();

        std::vector<std::unique_ptr<type_declaration>> types;           // the type declarations..
        std::vector<std::unique_ptr<predicate_declaration>> predicates; // the predicate declarations..
        std::vector<std::unique_ptr<method_declaration>> methods;       // the method declarations..
        std::vector<std::unique_ptr<statement>> statements;             // the statements..

        while (tk->sym != EOF_ID)
            switch (tk->sym)
            {
            case TYPEDEF_ID:
                types.emplace_back(parse_typedef_declaration());
                break;
            case ENUM_ID:
                types.emplace_back(parse_enum_declaration());
                break;
            case CLASS_ID:
                types.emplace_back(parse_class_declaration());
                break;
            case PREDICATE_ID:
                predicates.emplace_back(parse_predicate_declaration());
                break;
            case VOID_ID:
                methods.emplace_back(parse_method_declaration());
                break;
            case BOOL_ID:
            case INT_ID:
            case REAL_ID:
            case TIME_ID:
            case STRING_ID:
            case LBRACE_ID:
            case BANG_ID:
            case FACT_ID:
            case GOAL_ID:
            case BoolLiteral_ID:
            case IntLiteral_ID:
            case RealLiteral_ID:
                statements.emplace_back(parse_statement());
                break;
            case ID_ID:
            {
                size_t c_pos = pos;
                tk = next_token();
                while (match(DOT_ID))
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");
                }
                if (match(ID_ID) && match(LPAREN_ID))
                {
                    backtrack(c_pos);
                    methods.emplace_back(parse_method_declaration());
                }
                else
                {
                    backtrack(c_pos);
                    statements.emplace_back(parse_statement());
                }
                break;
            }
            default:
                error("Expected either `typedef` or `enum` or `class` or `predicate` or `void` or identifier..");
            }

        return std::make_unique<compilation_unit>(std::move(types), std::move(methods), std::move(predicates), std::move(statements));
    }

    std::unique_ptr<typedef_declaration> parser::parse_typedef_declaration()
    {
        switch (tk->sym)
        {
        case BOOL_ID:
        case INT_ID:
        case REAL_ID:
        case TIME_ID:
        case STRING_ID:
        {
            id_token primitive_type(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
            tk = next_token();

            std::unique_ptr<expression> expr = parse_expression();

            if (!match(ID_ID))
                error("Expected identifier..");

            auto name = *static_cast<const id_token *>(tokens[pos - 2].get());

            if (!match(SEMICOLON_ID))
                error("Expected `;`..");

            return std::make_unique<typedef_declaration>(std::move(name), std::move(primitive_type), std::move(expr));
        }
        default:
            error("Expected either `bool` or `int` or `real` or `time` or `string`..");
            return nullptr;
        }
    }
    std::unique_ptr<enum_declaration> parser::parse_enum_declaration()
    {
        std::vector<string_token> values;             // the values of the enum..
        std::vector<std::vector<id_token>> enum_refs; // the enum references..

        if (!match(ENUM_ID))
            error("Expected `enum`..");

        if (!match(ID_ID))
            error("Expected identifier..");

        auto name = *static_cast<const id_token *>(tokens[pos - 2].get());

        do
        {
            switch (tk->sym)
            {
            case LBRACE_ID:
            {
                tk = next_token();
                if (!match(StringLiteral_ID))
                    error("Expected string literal..");
                values.emplace_back(*static_cast<const string_token *>(tokens[pos - 2].get()));

                while (match(COMMA_ID))
                {
                    if (!match(StringLiteral_ID))
                        error("Expected string literal..");
                    values.emplace_back(*static_cast<const string_token *>(tokens[pos - 2].get()));
                }

                if (!match(RBRACE_ID))
                    error("Expected `}`..");
                break;
            }
            case ID_ID:
            {
                std::vector<id_token> ids;
                ids.emplace_back(*static_cast<const id_token *>(tk));
                tk = next_token();
                while (match(DOT_ID))
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                }
                enum_refs.emplace_back(std::move(ids));
                break;
            }
            default:
                error("Expected either `{` or identifier..");
            }
        } while (match(BAR_ID));

        if (!match(SEMICOLON_ID))
            error("Expected `;`..");

        return std::make_unique<enum_declaration>(std::move(name), std::move(values), std::move(enum_refs));
    }
    std::unique_ptr<class_declaration> parser::parse_class_declaration()
    {
        std::vector<std::vector<id_token>> base_classes;                    // the base classes of the class..
        std::vector<std::unique_ptr<field_declaration>> fields;             // the fields of the class..
        std::vector<std::unique_ptr<constructor_declaration>> constructors; // the constructors of the class..
        std::vector<std::unique_ptr<method_declaration>> methods;           // the methods of the class..
        std::vector<std::unique_ptr<predicate_declaration>> predicates;     // the predicates of the class..
        std::vector<std::unique_ptr<type_declaration>> types;               // the types of the class..

        if (!match(CLASS_ID))
            error("Expected `class`..");

        if (!match(ID_ID))
            error("Expected identifier..");

        auto name = *static_cast<const id_token *>(tokens[pos - 2].get());

        if (match(COLON_ID))
            do
            {
                std::vector<id_token> ids;
                do
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                } while (match(DOT_ID));
                base_classes.emplace_back(std::move(ids));
            } while (match(COMMA_ID));

        if (!match(LBRACE_ID))
            error("Expected `{`..");

        while (!match(RBRACE_ID))
            switch (tk->sym)
            {
            case TYPEDEF_ID:
                types.emplace_back(parse_typedef_declaration());
                break;
            case ENUM_ID:
                types.emplace_back(parse_enum_declaration());
                break;
            case CLASS_ID:
                types.emplace_back(parse_class_declaration());
                break;
            case PREDICATE_ID:
                predicates.emplace_back(parse_predicate_declaration());
                break;
            case VOID_ID:
                methods.emplace_back(parse_method_declaration());
                break;
            case BOOL_ID:
            case INT_ID:
            case REAL_ID:
            case TIME_ID:
            case STRING_ID: // either a primitive type method or a field declaration..
            {
                size_t c_pos = pos;
                tk = next_token();
                if (!match(ID_ID))
                    error("expected identifier..");
                switch (tk->sym)
                {
                case LPAREN_ID:
                    backtrack(c_pos);
                    methods.emplace_back(parse_method_declaration());
                    break;
                case EQ_ID:
                case COMMA_ID:
                case SEMICOLON_ID:
                    backtrack(c_pos);
                    fields.emplace_back(parse_field_declaration());
                    break;
                default:
                    error("expected either `(` or `=` or `;`..");
                }
                break;
            }
            case ID_ID: // either a constructor, a method or a field declaration..
            {
                size_t c_pos = pos;
                tk = next_token();
                switch (tk->sym)
                {
                case LPAREN_ID:
                    backtrack(c_pos);
                    constructors.emplace_back(parse_constructor_declaration());
                    break;
                case DOT_ID:
                    while (match(DOT_ID))
                        if (!match(ID_ID))
                            error("expected identifier..");
                    if (!match(ID_ID))
                        error("expected identifier..");
                    switch (tk->sym)
                    {
                    case LPAREN_ID:
                        backtrack(c_pos);
                        methods.emplace_back(parse_method_declaration());
                        break;
                    case EQ_ID:
                    case SEMICOLON_ID:
                        backtrack(c_pos);
                        fields.emplace_back(parse_field_declaration());
                        break;
                    default:
                        error("expected either `(` or `=` or `;`..");
                    }
                    break;
                case ID_ID:
                    tk = next_token();
                    switch (tk->sym)
                    {
                    case LPAREN_ID:
                        backtrack(c_pos);
                        methods.emplace_back(parse_method_declaration());
                        break;
                    case EQ_ID:
                    case SEMICOLON_ID:
                        backtrack(c_pos);
                        fields.emplace_back(parse_field_declaration());
                        break;
                    default:
                        error("expected either `(` or `=` or `;`..");
                    }
                    break;
                default:
                    error("expected either `(` or `.` or an identifier..");
                }
                break;
            }
            default:
                error("expected either `typedef` or `enum` or `class` or `predicate` or `void` or identifier..");
            }

        if (constructors.empty()) // if there are no constructors, we add the default constructor..
        {
            std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the parameters of the constructor..
            std::vector<init_element> inits;                                    // the initializations of the fields..
            std::vector<std::unique_ptr<statement>> body;                       // the body of the constructor..
            constructors.emplace_back(std::make_unique<constructor_declaration>(std::move(parameters), std::move(inits), std::move(body)));
        }

        return std::make_unique<class_declaration>(std::move(name), std::move(base_classes), std::move(fields), std::move(constructors), std::move(methods), std::move(predicates), std::move(types));
    }
    std::unique_ptr<field_declaration> parser::parse_field_declaration()
    {
        std::vector<id_token> tp;
        std::vector<init_element> inits;

        switch (tk->sym)
        {
        case BOOL_ID:
        case INT_ID:
        case REAL_ID:
        case TIME_ID:
        case STRING_ID:
            tp.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
            tk = next_token();
            break;
        case ID_ID:
            tp.emplace_back(*static_cast<const id_token *>(tk));
            tk = next_token();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                tp.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            }
            break;
        default:
            error("Expected either `bool` or `int` or `real` or `time` or `string`..");
        }

        do
        {
            if (!match(ID_ID))
                error("Expected identifier..");

            auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the field..

            if (match(EQ_ID))
            {
                std::vector<std::unique_ptr<expression>> args; // the arguments of the constructor..
                args.emplace_back(parse_expression());
                inits.emplace_back(std::move(name), std::move(args));
            }
            else if (match(LPAREN_ID))
            {
                std::vector<std::unique_ptr<expression>> args; // the arguments of the constructor..
                if (!match(RPAREN_ID))
                {
                    args.emplace_back(parse_expression());
                    while (match(COMMA_ID))
                        args.emplace_back(parse_expression());
                    if (!match(RPAREN_ID))
                        error("Expected `)`..");
                }
                inits.emplace_back(std::move(name), std::move(args));
            }
            else
                inits.emplace_back(std::move(name), std::vector<std::unique_ptr<expression>>());
        } while (match(COMMA_ID));

        if (!match(SEMICOLON_ID))
            error("Expected `;`..");

        return std::make_unique<field_declaration>(std::move(tp), std::move(inits));
    }
    std::unique_ptr<method_declaration> parser::parse_method_declaration()
    {
        std::vector<id_token> rt;
        std::vector<std::pair<std::vector<id_token>, id_token>> params;
        std::vector<std::unique_ptr<statement>> stmts;

        switch (tk->sym)
        {
        case VOID_ID:
        case BOOL_ID:
        case INT_ID:
        case REAL_ID:
        case TIME_ID:
        case STRING_ID:
            rt.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
            tk = next_token();
            break;
        case ID_ID:
            rt.emplace_back(*static_cast<const id_token *>(tk));
            tk = next_token();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                rt.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            }
            break;
        default:
            error("Expected either `void` or `bool` or `int` or `real` or `time` or `string` or identifier..");
        }

        if (!match(ID_ID))
            error("Expected identifier..");

        auto name = *static_cast<const id_token *>(tokens[pos - 2].get());

        if (!match(LPAREN_ID))
            error("Expected `(`..");

        if (!match(RPAREN_ID))
            do
            {
                std::vector<id_token> ids; // the identifiers of the type..
                switch (tk->sym)
                {
                case ID_ID:
                    ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next_token();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("Expected identifier..");
                        ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                    }
                    break;
                case BOOL_ID:
                case INT_ID:
                case REAL_ID:
                case TIME_ID:
                case STRING_ID:
                    ids.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
                    tk = next_token();
                    break;
                default:
                    error("Expected either identifier or `bool` or `int` or `real` or `time` or `string`..");
                }

                if (!match(ID_ID))
                    error("Expected identifier..");

                auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the parameter..

                params.emplace_back(std::move(ids), std::move(name));
            } while (match(COMMA_ID));

        if (!match(RPAREN_ID))
            error("Expected `)`..");

        if (!match(LBRACE_ID))
            error("Expected `{`..");

        while (!match(RBRACE_ID))
            stmts.emplace_back(parse_statement());

        return std::make_unique<method_declaration>(std::move(rt), std::move(name), std::move(params), std::move(stmts));
    }
    std::unique_ptr<constructor_declaration> parser::parse_constructor_declaration()
    {
        std::vector<std::pair<std::vector<id_token>, id_token>> parameters; // the parameters of the constructor..
        std::vector<init_element> inits;                                    // the initializations of the fields..
        std::vector<std::unique_ptr<statement>> body;                       // the body of the constructor..

        if (!match(ID_ID))
            error("Expected identifier..");

        if (!match(LPAREN_ID))
            error("Expected `(`..");

        if (!match(RPAREN_ID))
        { // parameters..
            do
            {
                std::vector<id_token> ids; // the identifiers of the type..
                switch (tk->sym)
                {
                case ID_ID:
                    ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next_token();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("Expected identifier..");
                        ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                    }
                    break;
                case BOOL_ID:
                case INT_ID:
                case REAL_ID:
                case TIME_ID:
                case STRING_ID:
                    ids.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
                    tk = next_token();
                    break;
                default:
                    error("Expected either identifier or `bool` or `int` or `real` or `time` or `string`..");
                }

                if (!match(ID_ID))
                    error("Expected identifier..");

                auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the parameter..

                parameters.emplace_back(std::move(ids), name);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("Expected `)`..");
        }

        if (match(COLON_ID)) // initializations..
            do
            {
                if (!match(ID_ID))
                    error("Expected identifier..");

                auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the field..

                if (!match(LPAREN_ID))
                    error("Expected `(`..");

                std::vector<std::unique_ptr<expression>> args; // the arguments of the constructor..
                if (!match(RPAREN_ID))
                {
                    args.emplace_back(parse_expression());
                    while (match(COMMA_ID))
                        args.emplace_back(parse_expression());
                    if (!match(RPAREN_ID))
                        error("Expected `)`..");
                }

                inits.emplace_back(std::move(name), std::move(args));
            } while (match(COMMA_ID));

        if (!match(LBRACE_ID))
            error("Expected `{`..");

        while (!match(RBRACE_ID))
            body.emplace_back(parse_statement());

        return std::make_unique<constructor_declaration>(std::move(parameters), std::move(inits), std::move(body));
    }
    std::unique_ptr<predicate_declaration> parser::parse_predicate_declaration()
    {
        std::vector<std::pair<std::vector<id_token>, id_token>> params;
        std::vector<std::vector<id_token>> base_predicates;
        std::vector<std::unique_ptr<statement>> stmts;

        if (!match(PREDICATE_ID))
            error("Expected `predicate`..");

        if (!match(ID_ID))
            error("Expected identifier..");

        auto name = *static_cast<const id_token *>(tokens[pos - 2].get());

        if (!match(LPAREN_ID))
            error("Expected `(`..");

        if (!match(RPAREN_ID))
        {
            do
            {
                std::vector<id_token> ids; // the identifiers of the type..
                switch (tk->sym)
                {
                case ID_ID:
                    ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next_token();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("Expected identifier..");
                        ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                    }
                    break;
                case BOOL_ID:
                case INT_ID:
                case REAL_ID:
                case TIME_ID:
                case STRING_ID:
                    ids.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
                    tk = next_token();
                    break;
                default:
                    error("Expected either identifier or `bool` or `int` or `real` or `time` or `string`..");
                }

                if (!match(ID_ID))
                    error("Expected identifier..");

                auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the parameter..

                params.emplace_back(std::move(ids), name);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("Expected `)`..");
        }

        if (match(COLON_ID))
            do
            {
                std::vector<id_token> ids;
                do
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                } while (match(DOT_ID));
                base_predicates.emplace_back(std::move(ids));
            } while (match(COMMA_ID));

        if (!match(LBRACE_ID))
            error("Expected `{`..");

        while (!match(RBRACE_ID))
            stmts.emplace_back(parse_statement());

        return std::make_unique<predicate_declaration>(std::move(name), std::move(params), std::move(base_predicates), std::move(stmts));
    }
    std::unique_ptr<statement> parser::parse_statement()
    {
        switch (tk->sym)
        {
        case BOOL_ID:
        case INT_ID:
        case REAL_ID:
        case TIME_ID:
        case STRING_ID:
        { // a local field having a primitive type..
            std::vector<id_token> field_type;
            std::vector<field_argument> fields;

            field_type.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
            tk = next_token();

            do
            { // the fields..
                if (!match(ID_ID))
                    error("Expected identifier..");

                auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the field..

                if (match(EQ_ID))
                    fields.emplace_back(std::move(name), parse_expression());
                else
                    fields.emplace_back(std::move(name));
            } while (match(COMMA_ID));

            if (!match(SEMICOLON_ID))
                error("Expected `;`..");

            return std::make_unique<local_field_statement>(std::move(field_type), std::move(fields));
        }
        case ID_ID:
        { // either a local field, an assignment or an expression..
            size_t c_pos = pos;
            std::vector<id_token> object_id;
            object_id.emplace_back(*static_cast<const id_token *>(tk));
            tk = next_token();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                object_id.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            }

            switch (tk->sym)
            {
            case ID_ID:
            { // a local field..
                std::vector<field_argument> fields;

                do
                { // the fields..
                    if (!match(ID_ID))
                        error("Expected identifier..");

                    auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the field..

                    if (match(EQ_ID))
                        fields.emplace_back(std::move(name), parse_expression());
                    else
                        fields.emplace_back(std::move(name));
                } while (match(COMMA_ID));

                if (!match(SEMICOLON_ID))
                    error("expected `;`..");

                return std::make_unique<local_field_statement>(std::move(object_id), std::move(fields));
            }
            case EQ_ID:
            { // an assignment..
                id_token field_name = object_id.back();
                object_id.pop_back();
                tk = next_token();
                std::unique_ptr<expression> expr = parse_expression();

                if (!match(SEMICOLON_ID))
                    error("Expected `;`..");

                return std::make_unique<assignment_statement>(std::move(object_id), std::move(field_name), std::move(expr));
            }
            case PLUS_ID: // an expression..
            case MINUS_ID:
            case STAR_ID:
            case SLASH_ID:
            case LT_ID:
            case LTEQ_ID:
            case EQEQ_ID:
            case GTEQ_ID:
            case GT_ID:
            case BANGEQ_ID:
            case IMPLICATION_ID:
            case BAR_ID:
            case AMP_ID:
            case CARET_ID:
            case SEMICOLON_ID:
            {
                backtrack(c_pos);
                std::unique_ptr<expression> expr = parse_expression();
                if (!match(SEMICOLON_ID))
                    error("Expected `;`..");
                return std::make_unique<expression_statement>(std::move(expr));
            }
            default:
                error("Expected either identifier or `=` or `+` or `-` or `*` or `/` or `<` or `<=` or `==` or `>=` or `>` or `!=` or `=>` or `|` or `&` or `^` or `;` or identifier..");
            }
            break;
        }
        case LBRACE_ID:
        { // either a block or a disjunction..
            std::vector<std::unique_ptr<conjunction_statement>> conjuncts;
            do
            {
                std::vector<std::unique_ptr<statement>> stmts;
                if (!match(LBRACE_ID))
                    error("Expected `{`..");
                while (!match(RBRACE_ID))
                    stmts.emplace_back(parse_statement());

                if (match(LBRACKET_ID))
                { // a priced conjunct..
                    std::unique_ptr<expression> cst = parse_expression();
                    if (!match(RBRACKET_ID))
                        error("Expected `]`..");
                    conjuncts.emplace_back(std::make_unique<conjunction_statement>(std::move(stmts), std::move(cst)));
                }
                else
                    conjuncts.emplace_back(std::make_unique<conjunction_statement>(std::move(stmts)));
            } while (match(OR_ID));

            if (conjuncts.size() == 1)
                return std::move(conjuncts[0]);

            return std::make_unique<disjunction_statement>(std::move(conjuncts));
        }
        case FOR_ID:
        { // a for loop..
            if (!match(LPAREN_ID))
                error("Expected `(`..");

            std::vector<id_token> enum_type;
            std::vector<std::unique_ptr<statement>> statements;

            do // the enum type..
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                enum_type.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            } while (match(DOT_ID));

            if (!match(ID_ID))
                error("Expected identifier..");

            auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the enum..

            if (!match(RPAREN_ID))
                error("Expected `)`..");

            if (!match(LBRACE_ID))
                error("Expected `{`..");

            while (!match(RBRACE_ID))
                statements.emplace_back(parse_statement());

            return std::make_unique<for_all_statement>(std::move(enum_type), std::move(name), std::move(statements));
        }
        case RETURN_ID:
        { // a return statement..
            tk = next_token();
            std::unique_ptr<expression> expr = parse_expression();
            if (!match(SEMICOLON_ID))
                error("Expected `;`..");
            return std::make_unique<return_statement>(std::move(expr));
        }
        case FACT_ID:
        case GOAL_ID:
        { // either a fact or a goal..
            bool is_fact = tk->sym == FACT_ID;
            tk = next_token();
            std::vector<id_token> formula_scope;
            std::vector<field_argument> arguments;

            if (!match(ID_ID))
                error("Expected identifier..");

            auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the formula..

            if (!match(EQ_ID))
                error("Expected `=`..");

            if (!match(NEW_ID))
                error("Expected `new`..");

            do // the formula scope..
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                formula_scope.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            } while (match(DOT_ID));

            auto predicate_name = formula_scope.back();
            formula_scope.pop_back();

            if (!match(LPAREN_ID))
                error("Expected `(`..");

            if (!match(RPAREN_ID))
            {
                do // the arguments..
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");

                    auto name = *static_cast<const id_token *>(tokens[pos - 2].get()); // the name of the argument..

                    if (match(COLON_ID))
                        arguments.emplace_back(std::move(name), parse_expression());
                    else
                        arguments.emplace_back(std::move(name));
                } while (match(COMMA_ID));

                if (!match(RPAREN_ID))
                    error("Expected `)`..");
            }

            if (!match(SEMICOLON_ID))
                error("Expected `;`..");

            return std::make_unique<formula_statement>(is_fact, std::move(name), std::move(formula_scope), std::move(predicate_name), std::move(arguments));
        }
        default:
        {
            auto xpr = parse_expression();
            if (!match(SEMICOLON_ID))
                error("Expected `;`..");
            return std::make_unique<expression_statement>(std::move(xpr));
        }
        }
        throw std::runtime_error("Unreachable code..");
    }
    std::unique_ptr<expression> parser::parse_expression(const size_t &pr)
    {
        std::unique_ptr<expression> expr;
        switch (tk->sym)
        {
        case BoolLiteral_ID:
            expr = std::make_unique<bool_expression>(*static_cast<const bool_token *>(tk));
            tk = next_token();
            break;
        case IntLiteral_ID:
            expr = std::make_unique<int_expression>(*static_cast<const int_token *>(tk));
            tk = next_token();
            break;
        case RealLiteral_ID:
            expr = std::make_unique<real_expression>(*static_cast<const real_token *>(tk));
            tk = next_token();
            break;
        case StringLiteral_ID:
            expr = std::make_unique<string_expression>(*static_cast<const string_token *>(tk));
            tk = next_token();
            break;
        case LPAREN_ID: // either a parenthesys expression or a cast..
        {
            tk = next_token();
            size_t c_pos = pos;
            do
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
            } while (match(DOT_ID));

            if (match(RPAREN_ID))
            { // cast..
                backtrack(c_pos);
                std::vector<id_token> tp;
                switch (tk->sym)
                {
                case BOOL_ID:
                case INT_ID:
                case REAL_ID:
                case TIME_ID:
                case STRING_ID:
                    tp.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
                    tk = next_token();
                    break;
                case ID_ID:
                    tp.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next_token();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("Expected identifier..");
                        tp.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                    }
                    break;
                default:
                    error("Expected either `bool` or `int` or `real` or `time` or `string` or identifier..");
                }

                if (!match(RPAREN_ID))
                    error("Expected `)`..");

                expr = std::make_unique<cast_expression>(std::move(tp), parse_expression(0));
            }
            else
            { // parenthesys expression..
                backtrack(c_pos);
                expr = parse_expression(0);
                if (!match(RPAREN_ID))
                    error("Expected `)`..");
            }
            break;
        }
        case PLUS_ID:
            tk = next_token();
            expr = std::make_unique<plus_expression>(parse_expression(4));
            break;
        case MINUS_ID:
            tk = next_token();
            expr = std::make_unique<minus_expression>(parse_expression(4));
            break;
        case BANG_ID:
            tk = next_token();
            expr = std::make_unique<not_expression>(parse_expression(4));
            break;
        case NEW_ID:
        {
            tk = next_token();
            std::vector<id_token> tp;
            switch (tk->sym)
            {
            case BOOL_ID:
            case INT_ID:
            case REAL_ID:
            case TIME_ID:
            case STRING_ID:
                tp.emplace_back(tk->to_string(), tk->start_line, tk->start_pos, tk->end_line, tk->end_pos);
                tk = next_token();
                break;
            case ID_ID:
                tp.emplace_back(*static_cast<const id_token *>(tk));
                tk = next_token();
                while (match(DOT_ID))
                {
                    if (!match(ID_ID))
                        error("Expected identifier..");
                    tp.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
                }
                break;
            default:
                error("Expected either `bool` or `int` or `real` or `time` or `string` or identifier..");
            }

            if (!match(LPAREN_ID))
                error("Expected `(`..");

            std::vector<std::unique_ptr<expression>> args;
            if (!match(RPAREN_ID))
            {
                args.emplace_back(parse_expression());
                while (match(COMMA_ID))
                    args.emplace_back(parse_expression());
                if (!match(RPAREN_ID))
                    error("Expected `)`..");
            }

            expr = std::make_unique<constructor_expression>(std::move(tp), std::move(args));
            break;
        }
        case ID_ID:
        {
            std::vector<id_token> object_id;
            object_id.emplace_back(*static_cast<const id_token *>(tk));
            tk = next_token();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("Expected identifier..");
                object_id.emplace_back(*static_cast<const id_token *>(tokens[pos - 2].get()));
            }
            if (match(LPAREN_ID))
            {
                std::vector<std::unique_ptr<expression>> args;
                if (!match(RPAREN_ID))
                {
                    args.emplace_back(parse_expression());
                    while (match(COMMA_ID))
                        args.emplace_back(parse_expression());
                    if (!match(RPAREN_ID))
                        error("Expected `)`..");
                }
                id_token name = object_id.back();
                object_id.pop_back();
                expr = std::make_unique<function_expression>(std::move(object_id), std::move(name), std::move(args));
            }
            else
                expr = std::make_unique<id_expression>(std::move(object_id));
            break;
        }
        default:
            error("Expected either a literal or `(` or `+` or `-` or `!` or `new` or identifier..");
        }

        while (
            ((tk->sym == EQEQ_ID || tk->sym == BANGEQ_ID) && 0 >= pr) ||
            ((tk->sym == LT_ID || tk->sym == LTEQ_ID || tk->sym == GTEQ_ID || tk->sym == GT_ID || tk->sym == IMPLICATION_ID || tk->sym == BAR_ID || tk->sym == AMP_ID || tk->sym == CARET_ID) && 1 >= pr) ||
            ((tk->sym == PLUS_ID || tk->sym == MINUS_ID) && 2 >= pr) ||
            ((tk->sym == STAR_ID || tk->sym == SLASH_ID) && 3 >= pr))
        {
            switch (tk->sym)
            {
            case EQEQ_ID:
                assert(0 >= pr);
                tk = next_token();
                expr = std::make_unique<eq_expression>(std::move(expr), parse_expression(1));
                break;
            case BANGEQ_ID:
                assert(0 >= pr);
                tk = next_token();
                expr = std::make_unique<neq_expression>(std::move(expr), parse_expression(1));
                break;
            case LT_ID:
            {
                assert(1 >= pr);
                tk = next_token();
                expr = std::make_unique<lt_expression>(std::move(expr), parse_expression(1));
                break;
            }
            case LTEQ_ID:
            {
                assert(1 >= pr);
                tk = next_token();
                expr = std::make_unique<leq_expression>(std::move(expr), parse_expression(1));
                break;
            }
            case GTEQ_ID:
            {
                assert(1 >= pr);
                tk = next_token();
                expr = std::make_unique<geq_expression>(std::move(expr), parse_expression(1));
                break;
            }
            case GT_ID:
            {
                assert(1 >= pr);
                tk = next_token();
                expr = std::make_unique<gt_expression>(std::move(expr), parse_expression(1));
                break;
            }
            case IMPLICATION_ID:
            {
                assert(1 >= pr);
                tk = next_token();
                expr = std::make_unique<implication_expression>(std::move(expr), parse_expression(1));
                break;
            }
            case BAR_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(BAR_ID))
                    xprs.emplace_back(parse_expression(2));

                expr = std::make_unique<disjunction_expression>(std::move(xprs));
                break;
            }
            case AMP_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(AMP_ID))
                    xprs.emplace_back(parse_expression(2));

                expr = std::make_unique<conjunction_expression>(std::move(xprs));
                break;
            }
            case CARET_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(CARET_ID))
                    xprs.emplace_back(parse_expression(2));

                expr = std::make_unique<xor_expression>(std::move(xprs));
                break;
            }
            case PLUS_ID:
            {
                assert(2 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(PLUS_ID))
                    xprs.emplace_back(parse_expression(3));

                expr = std::make_unique<addition_expression>(std::move(xprs));
                break;
            }
            case MINUS_ID:
            {
                assert(2 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(MINUS_ID))
                    xprs.emplace_back(parse_expression(3));

                expr = std::make_unique<subtraction_expression>(std::move(xprs));
                break;
            }
            case STAR_ID:
            {
                assert(3 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(STAR_ID))
                    xprs.emplace_back(parse_expression(4));

                expr = std::make_unique<multiplication_expression>(std::move(xprs));
                break;
            }
            case SLASH_ID:
            {
                assert(3 >= pr);
                std::vector<std::unique_ptr<expression>> xprs;
                xprs.emplace_back(std::move(expr));

                while (match(SLASH_ID))
                    xprs.emplace_back(parse_expression(4));

                expr = std::make_unique<division_expression>(std::move(xprs));
                break;
            }
            default:
                error("Expected either `==` or `!=` or `<` or `<=` or `>=` or `>` or `=>` or `|` or `&` or `^` or `+` or `-` or `*` or `/`..");
            }
        }

        return expr;
    }

    const token *parser::next_token()
    {
        while (pos >= tokens.size())
        {
            auto c_tk = lex.next_token();
            tokens.emplace_back(std::move(c_tk));
        }
        return tokens[pos++].get();
    }

    bool parser::match(const symbol &sym)
    {
        if (tk->sym == sym)
        {
            tk = next_token();
            return true;
        }
        else
            return false;
    }

    void parser::backtrack(const size_t &p) noexcept
    {
        pos = p;
        tk = tokens[pos - 1].get();
    }

    void parser::error(const std::string &err)
    {
        std::string last_five;
        for (size_t i = pos - 5; i < pos; ++i)
            if (i < tokens.size())
                last_five += tokens[i]->to_string() + " ";
        throw std::invalid_argument("[" + std::to_string(tk->start_line + 1) + ", " + std::to_string(tk->start_pos + 1) + "] " + err + "\n" + last_five);
    }
} // namespace riddle