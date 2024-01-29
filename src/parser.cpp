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

        if (constructors.empty())
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
    std::unique_ptr<predicate_declaration> parser::parse_predicate_declaration() {}
    std::unique_ptr<statement> parser::parse_statement() {}
    std::unique_ptr<expression> parser::parse_expression(const size_t &pr) {}

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