#include "parser.hpp"

namespace riddle
{
    std::unique_ptr<compilation_unit> parser::parse()
    {
        tk = next_token();

        std::vector<std::unique_ptr<type_declaration>> types;
        std::vector<std::unique_ptr<predicate_declaration>> predicates;
        std::vector<std::unique_ptr<method_declaration>> methods;
        std::vector<std::unique_ptr<statement>> statements;

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
                        error("expected identifier..");
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
                error("expected either `typedef` or `enum` or `class` or `predicate` or `void` or identifier..");
            }

        return std::make_unique<compilation_unit>(std::move(types), std::move(methods), std::move(predicates), std::move(statements));
    }

    std::unique_ptr<typedef_declaration> parser::parse_typedef_declaration() {}
    std::unique_ptr<enum_declaration> parser::parse_enum_declaration() {}
    std::unique_ptr<class_declaration> parser::parse_class_declaration() {}
    std::unique_ptr<field_declaration> parser::parse_field_declaration() {}
    std::unique_ptr<method_declaration> parser::parse_method_declaration() {}
    std::unique_ptr<constructor_declaration> parser::parse_constructor_declaration() {}
    std::unique_ptr<predicate_declaration> parser::parse_predicate_declaration() {}
    std::unique_ptr<statement> parser::parse_statement() {}
    std::unique_ptr<expression> parser::parse_expression(const size_t &pr) {}

    token *parser::next_token()
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