#include "riddle_parser.h"
#include <cassert>

namespace riddle
{
    using namespace ast;

    RIDDLE_EXPORT parser::parser(const std::string &str) : lex(str) {}
    RIDDLE_EXPORT parser::parser(std::istream &is) : lex(is) {}
    RIDDLE_EXPORT parser::~parser() {}

    const token *parser::next()
    {
        while (pos >= tks.size())
        {
            auto c_tk = lex.next();
            tks.emplace_back(std::move(c_tk));
        }
        return tks[pos++].get();
    }

    bool parser::match(const symbol &sym)
    {
        if (tk->sym == sym)
        {
            tk = next();
            return true;
        }
        else
            return false;
    }

    void parser::backtrack(const size_t &p) noexcept
    {
        pos = p;
        tk = tks[pos - 1].get();
    }

    RIDDLE_EXPORT std::unique_ptr<const compilation_unit> parser::parse()
    {
        tk = next();

        std::vector<std::unique_ptr<const type_declaration>> ts;
        std::vector<std::unique_ptr<const method_declaration>> ms;
        std::vector<std::unique_ptr<const predicate_declaration>> ps;
        std::vector<std::unique_ptr<const statement>> stmnts;

        while (tk->sym != EOF_ID)
        {
            switch (tk->sym)
            {
            case TYPEDEF_ID:
                ts.emplace_back(_typedef_declaration());
                break;
            case ENUM_ID:
                ts.emplace_back(_enum_declaration());
                break;
            case CLASS_ID:
                ts.emplace_back(_class_declaration());
                break;
            case PREDICATE_ID:
                ps.emplace_back(_predicate_declaration());
                break;
            case VOID_ID:
                ms.emplace_back(_method_declaration());
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
                stmnts.emplace_back(_statement());
                break;
            case ID_ID:
            {
                size_t c_pos = pos;
                tk = next();
                while (match(DOT_ID))
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                }
                if (match(ID_ID) && match(LPAREN_ID))
                {
                    backtrack(c_pos);
                    ms.emplace_back(_method_declaration());
                }
                else
                {
                    backtrack(c_pos);
                    stmnts.emplace_back(_statement());
                }
                break;
            }
            default:
                error("expected either `typedef` or `enum` or `class` or `predicate` or `void` or identifier..");
            }
        }

        return new_compilation_unit(std::move(ms), std::move(ps), std::move(ts), std::move(stmnts));
    }

    std::unique_ptr<const typedef_declaration> parser::_typedef_declaration()
    {
        id_token *pt = nullptr;
        std::unique_ptr<const ast::expression> e;

        if (!match(TYPEDEF_ID))
            error("expected `typedef`..");

        switch (tk->sym)
        {
        case BOOL_ID:
            pt = new id_token(0, 0, 0, 0, BOOL_KW);
            break;
        case INT_ID:
            pt = new id_token(0, 0, 0, 0, INT_KW);
            break;
        case REAL_ID:
            pt = new id_token(0, 0, 0, 0, REAL_KW);
            break;
        case TIME_ID:
            pt = new id_token(0, 0, 0, 0, TIME_KW);
            break;
        case STRING_ID:
            pt = new id_token(0, 0, 0, 0, STRING_KW);
            break;
        default:
            error("expected primitive type..");
        }
        tk = next();

        e = _expression();

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        if (!match(SEMICOLON_ID))
            error("expected `;`..");

        return new_typedef_declaration(n, *pt, std::move(e));
    }

    std::unique_ptr<const enum_declaration> parser::_enum_declaration()
    {
        std::vector<string_token> es;
        std::vector<std::vector<id_token>> trs;

        if (!match(ENUM_ID))
            error("expected `enum`..");

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        do
        {
            switch (tk->sym)
            {
            case LBRACE_ID:
                tk = next();
                if (!match(StringLiteral_ID))
                    error("expected string literal..");
                es.emplace_back(*static_cast<const string_token *>(tks[pos - 2].get()));

                while (match(COMMA_ID))
                {
                    if (!match(StringLiteral_ID))
                        error("expected string literal..");
                    es.emplace_back(*static_cast<const string_token *>(tks[pos - 2].get()));
                }

                if (!match(RBRACE_ID))
                    error("expected `}`..");
                break;
            case ID_ID:
            {
                std::vector<id_token> ids;
                ids.emplace_back(*static_cast<const id_token *>(tk));
                tk = next();
                while (match(DOT_ID))
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                }
                trs.emplace_back(std::move(ids));
                break;
            }
            default:
                error("expected either `{` or identifier..");
            }
        } while (match(BAR_ID));

        if (!match(SEMICOLON_ID))
            error("expected `;`..");

        return new_enum_declaration(n, std::move(es), std::move(trs));
    }

    std::unique_ptr<const class_declaration> parser::_class_declaration()
    {
        std::vector<std::vector<id_token>> bcs;                         // the base classes..
        std::vector<std::unique_ptr<const field_declaration>> fs;       // the fields of the class..
        std::vector<std::unique_ptr<const constructor_declaration>> cs; // the constructors of the class..
        std::vector<std::unique_ptr<const method_declaration>> ms;      // the methods of the class..
        std::vector<std::unique_ptr<const predicate_declaration>> ps;   // the predicates of the class..
        std::vector<std::unique_ptr<const type_declaration>> ts;        // the types of the class..

        if (!match(CLASS_ID))
            error("expected `class`..");

        if (!match(ID_ID))
            error("expected identifier..");
        // the name of the class..
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        if (match(COLON_ID))
        {
            do
            {
                std::vector<id_token> ids;
                do
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                } while (match(DOT_ID));
                bcs.emplace_back(std::move(ids));
            } while (match(COMMA_ID));
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
        {
            switch (tk->sym)
            {
            case TYPEDEF_ID:
                ts.emplace_back(_typedef_declaration());
                break;
            case ENUM_ID:
                ts.emplace_back(_enum_declaration());
                break;
            case CLASS_ID:
                ts.emplace_back(_class_declaration());
                break;
            case PREDICATE_ID:
                ps.emplace_back(_predicate_declaration());
                break;
            case VOID_ID:
                ms.emplace_back(_method_declaration());
                break;
            case BOOL_ID:
            case INT_ID:
            case REAL_ID:
            case TIME_ID:
            case STRING_ID: // either a primitive type method or a field declaration..
            {
                size_t c_pos = pos;
                tk = next();
                if (!match(ID_ID))
                    error("expected identifier..");
                switch (tk->sym)
                {
                case LPAREN_ID:
                    backtrack(c_pos);
                    ms.emplace_back(_method_declaration());
                    break;
                case EQ_ID:
                case COMMA_ID:
                case SEMICOLON_ID:
                    backtrack(c_pos);
                    fs.emplace_back(_field_declaration());
                    break;
                default:
                    error("expected either `(` or `=` or `;`..");
                }
                break;
            }
            case ID_ID: // either a constructor, a method or a field declaration..
            {
                size_t c_pos = pos;
                tk = next();
                switch (tk->sym)
                {
                case LPAREN_ID:
                    backtrack(c_pos);
                    cs.emplace_back(_constructor_declaration());
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
                        ms.emplace_back(_method_declaration());
                        break;
                    case EQ_ID:
                    case SEMICOLON_ID:
                        backtrack(c_pos);
                        fs.emplace_back(_field_declaration());
                        break;
                    default:
                        error("expected either `(` or `=` or `;`..");
                    }
                    break;
                case ID_ID:
                    tk = next();
                    switch (tk->sym)
                    {
                    case LPAREN_ID:
                        backtrack(c_pos);
                        ms.emplace_back(_method_declaration());
                        break;
                    case EQ_ID:
                    case SEMICOLON_ID:
                        backtrack(c_pos);
                        fs.emplace_back(_field_declaration());
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
        }

        if (cs.empty()) // we add a default constructor..
            cs.emplace_back(new_constructor_declaration({}, {}, {}, {}));

        return new_class_declaration(n, std::move(bcs), std::move(fs), std::move(cs), std::move(ms), std::move(ps), std::move(ts));
    }

    std::unique_ptr<const field_declaration> parser::_field_declaration()
    {
        std::vector<id_token> tp;
        std::vector<std::unique_ptr<const variable_declaration>> ds;

        switch (tk->sym)
        {
        case BOOL_ID:
            tp.emplace_back(id_token(0, 0, 0, 0, BOOL_KW));
            tk = next();
            break;
        case INT_ID:
            tp.emplace_back(id_token(0, 0, 0, 0, INT_KW));
            tk = next();
            break;
        case REAL_ID:
            tp.emplace_back(id_token(0, 0, 0, 0, REAL_KW));
            tk = next();
            break;
        case TIME_ID:
            tp.emplace_back(id_token(0, 0, 0, 0, TIME_KW));
            tk = next();
            break;
        case STRING_ID:
            tp.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
            tk = next();
            break;
        case ID_ID:
            tp.emplace_back(*static_cast<const id_token *>(tk));
            tk = next();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                tp.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            }
            break;
        default:
            error("expected either `bool` or `int` or `real` or `string` or an identifier..");
        }

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        if (match(EQ_ID))
            ds.emplace_back(new_variable_declaration(n, _expression()));
        else
            ds.emplace_back(new_variable_declaration(n));

        while (match(COMMA_ID))
        {
            if (!match(ID_ID))
                error("expected identifier..");
            auto c_n = *static_cast<const id_token *>(tks[pos - 2].get());

            if (match(EQ_ID))
                ds.emplace_back(new_variable_declaration(c_n, _expression()));
            else
                ds.emplace_back(new_variable_declaration(c_n));
        }

        if (!match(SEMICOLON_ID))
            error("expected `;`..");

        return new_field_declaration(std::move(tp), std::move(ds));
    }

    std::unique_ptr<const method_declaration> parser::_method_declaration()
    {
        std::vector<id_token> rt;
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<std::unique_ptr<const ast::statement>> stmnts;

        if (!match(VOID_ID))
        {
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                rt.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            } while (match(DOT_ID));
        }

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        if (!match(LPAREN_ID))
            error("expected `(`..");

        if (!match(RPAREN_ID))
        {
            do
            {
                std::vector<id_token> p_ids;
                switch (tk->sym)
                {
                case BOOL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, BOOL_KW));
                    tk = next();
                    break;
                case INT_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, INT_KW));
                    tk = next();
                    break;
                case REAL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, REAL_KW));
                    tk = next();
                    break;
                case TIME_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_KW));
                    tk = next();
                    break;
                case STRING_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
                    tk = next();
                    break;
                case ID_ID:
                    p_ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("expected identifier..");
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                    }
                    break;
                default:
                    error("expected either `bool` or `int` or `real` or `string` or an identifier..");
                }
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].get());
                pars.emplace_back(p_ids, pn);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("expected `)`..");
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
            stmnts.emplace_back(_statement());

        return new_method_declaration(std::move(rt), n, std::move(pars), std::move(stmnts));
    }

    std::unique_ptr<const constructor_declaration> parser::_constructor_declaration()
    {
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<id_token> ins;
        std::vector<std::vector<std::unique_ptr<const expression>>> ivs;
        std::vector<std::unique_ptr<const ast::statement>> stmnts;

        if (!match(ID_ID))
            error("expected identifier..");

        if (!match(LPAREN_ID))
            error("expected `(`..");

        if (!match(RPAREN_ID))
        {
            do
            {
                std::vector<id_token> p_ids;
                switch (tk->sym)
                {
                case ID_ID:
                    p_ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("expected identifier..");
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                    }
                    break;
                case BOOL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, BOOL_KW));
                    tk = next();
                    break;
                case INT_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, INT_KW));
                    tk = next();
                    break;
                case REAL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, REAL_KW));
                    tk = next();
                    break;
                case TIME_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_KW));
                    tk = next();
                    break;
                case STRING_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
                    tk = next();
                    break;
                default:
                    error("expected either `bool` or `int` or `real` or `string` or an identifier..");
                }
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].get());
                pars.emplace_back(p_ids, pn);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("expected `)`..");
        }

        if (match(COLON_ID))
        {
            do
            {
                std::vector<std::unique_ptr<const expression>> xprs;
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].get());

                if (!match(LPAREN_ID))
                    error("expected `(`..");

                if (!match(RPAREN_ID))
                {
                    do
                    {
                        xprs.emplace_back(_expression());
                    } while (match(COMMA_ID));

                    if (!match(RPAREN_ID))
                        error("expected `)`..");
                }
                ins.emplace_back(pn);
                ivs.emplace_back(std::move(xprs));
            } while (match(COMMA_ID));
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
            stmnts.emplace_back(_statement());

        return new_constructor_declaration(pars, std::move(ins), std::move(ivs), std::move(stmnts));
    }

    std::unique_ptr<const predicate_declaration> parser::_predicate_declaration()
    {
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<std::vector<id_token>> pl;
        std::vector<std::unique_ptr<const ast::statement>> stmnts;

        if (!match(PREDICATE_ID))
            error("expected `predicate`..");

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].get());

        if (!match(LPAREN_ID))
            error("expected `(`..");

        if (!match(RPAREN_ID))
        {
            do
            {
                std::vector<id_token> p_ids;
                switch (tk->sym)
                {
                case BOOL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, BOOL_KW));
                    tk = next();
                    break;
                case INT_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, INT_KW));
                    tk = next();
                    break;
                case REAL_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, REAL_KW));
                    tk = next();
                    break;
                case TIME_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_KW));
                    tk = next();
                    break;
                case STRING_ID:
                    p_ids.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
                    tk = next();
                    break;
                case ID_ID:
                    p_ids.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next();
                    while (match(DOT_ID))
                    {
                        if (!match(ID_ID))
                            error("expected identifier..");
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                    }
                    break;
                default:
                    error("expected either `bool` or `int` or `real` or `string` or an identifier..");
                }
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].get());
                pars.emplace_back(p_ids, pn);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("expected `)`..");
        }

        if (match(COLON_ID))
        {
            do
            {
                std::vector<id_token> p_ids;
                do
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                } while (match(DOT_ID));
                pl.emplace_back(std::move(p_ids));
            } while (match(COMMA_ID));
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
            stmnts.emplace_back(_statement());

        return new_predicate_declaration(n, std::move(pars), std::move(pl), std::move(stmnts));
    }

    std::unique_ptr<const statement> parser::_statement()
    {
        switch (tk->sym)
        {
        case BOOL_ID:
        case INT_ID:
        case REAL_ID:
        case TIME_ID:
        case STRING_ID: // a local field having a primitive type..
        {
            std::vector<id_token> ft;
            switch (tk->sym)
            {
            case BOOL_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, BOOL_KW));
                break;
            case INT_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, INT_KW));
                break;
            case REAL_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, REAL_KW));
                break;
            case TIME_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, TIME_KW));
                break;
            case STRING_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
                break;
            default:
                error("expected either `bool` or `int` or `real` or `string`..");
            }
            tk = next();

            std::vector<id_token> ns;
            std::vector<std::unique_ptr<const expression>> es;

            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                ns.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));

                if (tk->sym == EQ_ID)
                {
                    tk = next();
                    es.emplace_back(_expression());
                }
                else
                    es.emplace_back(nullptr);
            } while (match(COMMA_ID));

            if (!match(SEMICOLON_ID))
                error("expected `;`..");

            return new_local_field_statement(std::move(ft), std::move(ns), std::move(es));
        }
        case ID_ID: // either a local field, an assignment or an expression..
        {
            size_t c_pos = pos;
            std::vector<id_token> is;
            is.emplace_back(*static_cast<const id_token *>(tk));
            tk = next();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                is.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            }

            switch (tk->sym)
            {
            case ID_ID: // a local field..
            {
                std::vector<id_token> ns;
                std::vector<std::unique_ptr<const expression>> es;

                do
                {
                    ns.emplace_back(*static_cast<const id_token *>(tk));
                    tk = next();
                    if (tk->sym == EQ_ID)
                    {
                        tk = next();
                        es.emplace_back(_expression());
                    }
                    else
                        es.emplace_back(nullptr);
                } while (match(COMMA_ID));

                if (!match(SEMICOLON_ID))
                    error("expected `;`..");

                return new_local_field_statement(std::move(is), std::move(ns), std::move(es));
            }
            case EQ_ID: // an assignment..
            {
                id_token i = is.back();
                is.pop_back();
                tk = next();
                std::unique_ptr<const ast::expression> e = _expression();
                if (!match(SEMICOLON_ID))
                    error("expected `;`..");
                return new_assignment_statement(std::move(is), i, std::move(e));
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
                std::unique_ptr<const ast::expression> e = _expression();
                if (!match(SEMICOLON_ID))
                    error("expected `;`..");
                return new_expression_statement(std::move(e));
            }
            default:
                error("expected either `=` or an identifier..");
                return nullptr;
            }
        }
        case LBRACE_ID: // either a block or a disjunction..
        {
            tk = next();
            std::vector<std::unique_ptr<const ast::statement>> stmnts;
            do
            {
                stmnts.emplace_back(_statement());
            } while (!match(RBRACE_ID));
            switch (tk->sym)
            {
            case LBRACKET_ID:
            case OR_ID: // a disjunctive statement..
            {
                std::vector<std::vector<std::unique_ptr<const ast::statement>>> conjs;
                std::vector<std::unique_ptr<const expression>> conj_costs;
                std::unique_ptr<const ast::expression> e = nullptr;
                if (match(LBRACKET_ID))
                {
                    e = _expression();
                    if (!match(RBRACKET_ID))
                        error("expected `]`..");
                }
                conjs.emplace_back(std::move(stmnts));
                conj_costs.emplace_back(std::move(e));
                while (match(OR_ID))
                {
                    if (!match(LBRACE_ID))
                        error("expected `{`..");
                    while (!match(RBRACE_ID))
                        stmnts.emplace_back(_statement());
                    if (match(LBRACKET_ID))
                    {
                        e = _expression();
                        if (!match(RBRACKET_ID))
                            error("expected `]`..");
                    }
                    conjs.emplace_back(std::move(stmnts));
                    conj_costs.emplace_back(std::move(e));
                }
                return new_disjunction_statement(std::move(conjs), std::move(conj_costs));
            }
            default: // a conjunction statement..
                return new_conjunction_statement(std::move(stmnts));
            }
        }
        case FACT_ID:
        case GOAL_ID:
        {
            bool isf = tk->sym == FACT_ID;
            tk = next();
            std::vector<id_token> scp;
            std::vector<id_token> assn_ns;
            std::vector<std::unique_ptr<const expression>> assn_vs;

            if (!match(ID_ID))
                error("expected identifier..");
            auto fn = *static_cast<const id_token *>(tks[pos - 2].get());

            if (!match(EQ_ID))
                error("expected `=`..");

            if (!match(NEW_ID))
                error("expected `new`..");

            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                scp.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            } while (match(DOT_ID));

            auto pn = scp.back();
            scp.pop_back();

            if (!match(LPAREN_ID))
                error("expected `(`..");

            if (!match(RPAREN_ID))
            {
                do
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    id_token assgn_name = *static_cast<const id_token *>(tks[pos - 2].get());

                    if (!match(COLON_ID))
                        error("expected `:`..");

                    assn_ns.emplace_back(assgn_name);
                    assn_vs.emplace_back(_expression());
                } while (match(COMMA_ID));

                if (!match(RPAREN_ID))
                    error("expected `)`..");
            }

            if (!match(SEMICOLON_ID))
                error("expected `;`..");
            return new_formula_statement(isf, fn, scp, pn, assn_ns, std::move(assn_vs));
        }
        case RETURN_ID:
        {
            std::unique_ptr<const ast::expression> e = _expression();
            if (!match(SEMICOLON_ID))
                error("expected `;`..");
            return new_return_statement(std::move(e));
        }
        default:
        {
            std::unique_ptr<const ast::expression> xpr = _expression();
            if (!match(SEMICOLON_ID))
                error("expected `;`..");
            return new_expression_statement(std::move(xpr));
        }
        }
    }

    std::unique_ptr<const ast::expression> parser::_expression(const size_t &pr)
    {
        std::unique_ptr<const expression> e = nullptr;
        switch (tk->sym)
        {
        case BoolLiteral_ID:
            tk = next();
            e = new_bool_literal_expression(*static_cast<const bool_token *>(tks[pos - 2].get()));
            break;
        case IntLiteral_ID:
            tk = next();
            e = new_int_literal_expression(*static_cast<const int_token *>(tks[pos - 2].get()));
            break;
        case RealLiteral_ID:
            tk = next();
            e = new_real_literal_expression(*static_cast<const real_token *>(tks[pos - 2].get()));
            break;
        case StringLiteral_ID:
            tk = next();
            e = new_string_literal_expression(*static_cast<const string_token *>(tks[pos - 2].get()));
            break;
        case LPAREN_ID: // either a parenthesys expression or a cast..
        {
            tk = next();

            size_t c_pos = pos;
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
            } while (match(DOT_ID));

            if (match(RPAREN_ID)) // a cast..
            {
                backtrack(c_pos);
                std::vector<id_token> ids;
                do
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
                } while (match(DOT_ID));

                if (!match(RPAREN_ID))
                    error("expected `)`..");
                e = new_cast_expression(std::move(ids), _expression());
            }
            else // a parenthesis..
            {
                backtrack(c_pos);
                std::unique_ptr<const ast::expression> xpr = _expression();
                if (!match(RPAREN_ID))
                    error("expected `)`..");
                e = std::move(xpr);
            }
            break;
        }
        case PLUS_ID:
            tk = next();
            e = new_plus_expression(_expression(4));
            break;
        case MINUS_ID:
            tk = next();
            e = new_minus_expression(_expression(4));
            break;
        case BANG_ID:
            tk = next();
            e = new_not_expression(_expression(4));
            break;
        case NEW_ID:
        {
            tk = next();
            std::vector<id_token> ids;
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            } while (match(DOT_ID));

            std::vector<std::unique_ptr<const expression>> xprs;
            if (!match(LPAREN_ID))
                error("expected `(`..");

            if (!match(RPAREN_ID))
            {
                do
                {
                    xprs.emplace_back(_expression());
                } while (match(COMMA_ID));

                if (!match(RPAREN_ID))
                    error("expected `)`..");
            }

            e = new_constructor_expression(std::move(ids), std::move(xprs));
            break;
        }
        case ID_ID:
        {
            std::vector<id_token> is;
            is.emplace_back(*static_cast<const id_token *>(tk));
            tk = next();
            while (match(DOT_ID))
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                is.emplace_back(*static_cast<const id_token *>(tks[pos - 2].get()));
            }
            if (match(LPAREN_ID))
            {
                tk = next();
                id_token fn = is.back();
                is.pop_back();
                std::vector<std::unique_ptr<const expression>> xprs;
                if (!match(LPAREN_ID))
                    error("expected `(`..");

                if (!match(RPAREN_ID))
                {
                    do
                    {
                        xprs.emplace_back(_expression());
                    } while (match(COMMA_ID));

                    if (!match(RPAREN_ID))
                        error("expected `)`..");
                }

                e = new_function_expression(std::move(is), fn, std::move(xprs));
            }
            else
                e = new_id_expression(std::move(is));
            break;
        }
        default:
            error("expected either `(` or `+` or `-` or `!` or `[` or `new` or a literal or an identifier..");
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
                tk = next();
                e = new_eq_expression(std::move(e), _expression(1));
                break;
            case BANGEQ_ID:
                assert(0 >= pr);
                tk = next();
                e = new_neq_expression(std::move(e), _expression(1));
                break;
            case LT_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new_lt_expression(std::move(e), _expression(1));
                break;
            }
            case LTEQ_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new_leq_expression(std::move(e), _expression(1));
                break;
            }
            case GTEQ_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new_geq_expression(std::move(e), _expression(1));
                break;
            }
            case GT_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new_gt_expression(std::move(e), _expression(1));
                break;
            }
            case IMPLICATION_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new_implication_expression(std::move(e), _expression(1));
                break;
            }
            case BAR_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(BAR_ID))
                    xprs.emplace_back(_expression(2));

                e = new_disjunction_expression(std::move(xprs));
                break;
            }
            case AMP_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(AMP_ID))
                    xprs.emplace_back(_expression(2));

                e = new_conjunction_expression(std::move(xprs));
                break;
            }
            case CARET_ID:
            {
                assert(1 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(CARET_ID))
                    xprs.emplace_back(_expression(2));

                e = new_exct_one_expression(std::move(xprs));
                break;
            }
            case PLUS_ID:
            {
                assert(2 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(PLUS_ID))
                    xprs.emplace_back(_expression(3));

                e = new_addition_expression(std::move(xprs));
                break;
            }
            case MINUS_ID:
            {
                assert(2 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(MINUS_ID))
                    xprs.emplace_back(_expression(3));

                e = new_subtraction_expression(std::move(xprs));
                break;
            }
            case STAR_ID:
            {
                assert(3 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(STAR_ID))
                    xprs.emplace_back(_expression(4));

                e = new_multiplication_expression(std::move(xprs));
                break;
            }
            case SLASH_ID:
            {
                assert(3 >= pr);
                std::vector<std::unique_ptr<const expression>> xprs;
                xprs.emplace_back(std::move(e));

                while (match(SLASH_ID))
                    xprs.emplace_back(_expression(4));

                e = new_division_expression(std::move(xprs));
                break;
            }
            default:
                error("expected either `==` or `!=` or `<` or `<=` or `>` or `>=` or `->` or `|` or `&` or `^` or `+` or `-` or `*` or `/`..");
            }
        }

        return e;
    }

    void parser::error(const std::string &err) { throw std::invalid_argument("[" + std::to_string(tk->start_line + 1) + ", " + std::to_string(tk->start_pos + 1) + "] " + err); }
} // namespace riddle