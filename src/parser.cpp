#include "parser.h"
#include "core.h"
#include "item.h"
#include "constructor.h"
#include "method.h"
#include "conjunction.h"
#include <queue>
#include <cassert>

namespace riddle
{
    using namespace ast;

    expr bool_literal_expression::evaluate(scope &scp, env &) const { return scp.get_core().new_bool(literal.val); }
    expr int_literal_expression::evaluate(scope &scp, env &) const { return scp.get_core().new_int(literal.val); }
    expr real_literal_expression::evaluate(scope &scp, env &) const { return scp.get_core().new_real(literal.val); }
    expr string_literal_expression::evaluate(scope &scp, env &) const { return scp.get_core().new_string(literal.str); }

    expr cast_expression::evaluate(scope &scp, env &ctx) const { return xpr->evaluate(scp, ctx); }

    expr plus_expression::evaluate(scope &scp, env &ctx) const { return xpr->evaluate(scp, ctx); }
    expr minus_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().minus(xpr->evaluate(scp, ctx)); }

    expr not_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().negate(xpr->evaluate(scp, ctx)); }

    expr constructor_expression::evaluate(scope &scp, env &ctx) const
    {
        complex_type *t = dynamic_cast<complex_type *>(&scp.get_type(instance_type.front().id));
        if (!t)
            throw std::runtime_error("cannot invoke constructor on non-complex type");
        for (auto it = instance_type.begin() + 1; it != instance_type.end(); ++it)
            if (auto ct = dynamic_cast<complex_type *>(t))
                t = dynamic_cast<complex_type *>(&ct->get_type(it->id));
            else
                throw std::runtime_error("cannot invoke constructor on non-complex type");

        std::vector<expr> args;
        args.reserve(expressions.size());
        std::vector<std::reference_wrapper<type>> arg_types;
        arg_types.reserve(expressions.size());
        for (auto &xpr : expressions)
        {
            auto e = xpr->evaluate(scp, ctx);
            args.emplace_back(e);
            arg_types.emplace_back(e->get_type());
        }

        auto inst = t->new_instance();
        t->get_constructor(arg_types).call(inst, args);
        return inst;
    }

    expr eq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().eq(left->evaluate(scp, ctx), right->evaluate(scp, ctx)); }
    expr neq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().negate(scp.get_core().eq(left->evaluate(scp, ctx), right->evaluate(scp, ctx))); }

    expr lt_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().lt(left->evaluate(scp, ctx), right->evaluate(scp, ctx)); }
    expr leq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().leq(left->evaluate(scp, ctx), right->evaluate(scp, ctx)); }
    expr gt_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().gt(left->evaluate(scp, ctx), right->evaluate(scp, ctx)); }
    expr geq_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().geq(left->evaluate(scp, ctx), right->evaluate(scp, ctx)); }

    expr function_expression::evaluate(scope &scp, env &ctx) const
    {
        auto self = ctx.get(ids.front().id);
        for (auto it = ids.begin() + 1; it != ids.end(); ++it)
            if (auto c_env = dynamic_cast<env *>(self.operator->()))
                self = c_env->get(it->id);
            else
                throw std::runtime_error("cannot find `" + it->id + "` item..");

        std::vector<expr> args;
        std::vector<std::reference_wrapper<type>> arg_types;
        for (auto &xpr : expressions)
        {
            auto e = xpr->evaluate(scp, ctx);
            args.emplace_back(e);
            arg_types.emplace_back(e->get_type());
        }

        if (auto t = dynamic_cast<complex_type *>(&self->get_type()))
        {
            auto &m = t->get_method(function_name.id, arg_types);
            return m.call(self, args);
        }
        else
            throw std::runtime_error("cannot find function");
    }

    expr id_expression::evaluate(scope &, env &ctx) const
    {
        auto e = ctx.get(ids.front().id);
        for (auto it = ids.begin() + 1; it != ids.end(); ++it)
            if (auto c_env = dynamic_cast<env *>(e.operator->()))
                e = c_env->get(it->id);
            else
                throw std::runtime_error("cannot find `" + it->id + "` item..");
        return e;
    }

    expr implication_expression::evaluate(scope &scp, env &ctx) const { return scp.get_core().disj({scp.get_core().negate(left->evaluate(scp, ctx)), right->evaluate(scp, ctx)}); }

    expr disjunction_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().disj(args);
    }

    expr conjunction_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().conj(args);
    }

    expr exct_one_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().exct_one(args);
    }

    expr addition_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().add(args);
    }

    expr subtraction_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().sub(args);
    }

    expr multiplication_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().mul(args);
    }

    expr division_expression::evaluate(scope &scp, env &ctx) const
    {
        std::vector<expr> args;
        args.reserve(expressions.size());
        for (auto &xpr : expressions)
            args.emplace_back(xpr->evaluate(scp, ctx));
        return scp.get_core().div(args);
    }

    void local_field_statement::execute(scope &scp, env &ctx) const
    {
        for (size_t i = 0; i < names.size(); ++i)
            if (xprs[i])
                ctx.items.emplace(names[i].id, xprs[i]->evaluate(scp, ctx));
            else
            {
                type *t = &scp.get_type(field_type.front().id);
                for (auto it = field_type.begin() + 1; it != field_type.end(); ++it)
                    if (auto ct = dynamic_cast<complex_type *>(t))
                        t = &ct->get_type(it->id);
                    else
                        throw std::runtime_error("cannot find type `" + it->id + "`..");

                if (t->is_primitive())
                    ctx.items.emplace(names[i].id, t->new_instance());
                else if (auto ct = dynamic_cast<complex_type *>(t))
                    switch (ct->get_instances().size())
                    {
                    case 0:
                        throw inconsistency_exception();
                    case 1:
                        ctx.items.emplace(names[i].id, ct->get_instances().front());
                        break;
                    default:
                        ctx.items.emplace(names[i].id, scp.get_core().new_enum(*ct, ct->get_instances()));
                    }
                else if (auto et = dynamic_cast<enum_type *>(t))
                {
                    auto all_values = et->get_all_values();
                    switch (all_values.size())
                    {
                    case 0:
                        throw inconsistency_exception();
                    case 1:
                        ctx.items.emplace(names[i].id, all_values.front());
                        break;
                    default:
                        ctx.items.emplace(names[i].id, scp.get_core().new_enum(*et, all_values));
                    }
                }
                else if (auto td = dynamic_cast<typedef_type *>(t))
                    ctx.items.emplace(names[i].id, td->new_instance());
                else
                    throw std::runtime_error("cannot create instance of type `" + t->get_name() + "`..");
            }
    }

    void assignment_statement::execute(scope &scp, env &ctx) const
    {
        auto e = ctx.get(ids.front().id);
        for (auto it = ids.begin() + 1; it != ids.end(); ++it)
            if (auto c_env = dynamic_cast<env *>(e.operator->()))
                e = c_env->get(it->id);
            else
                throw std::runtime_error("cannot find `" + it->id + "` item..");

        if (auto c_env = dynamic_cast<env *>(e.operator->()))
            c_env->items.emplace(id.id, xpr->evaluate(scp, ctx));
        else
            throw std::runtime_error("cannot find `" + id.id + "` item..");
    }

    void expression_statement::execute(scope &scp, env &ctx) const { scp.get_core().assert_fact(xpr->evaluate(scp, ctx)); }

    void disjunction_statement::execute(scope &scp, env &ctx) const
    {
        std::vector<conjunction_ptr> conjs;
        conjs.reserve(conjunctions.size());

        for (size_t i = 0; i < conjunctions.size(); ++i)
            conjs.emplace_back(new conjunction(scp, ctx, conjunction_costs[i] ? scp.get_core().arith_value(conjunction_costs[i]->evaluate(scp, ctx)).get_rational() : utils::rational::ONE, conjunctions[i]));

        scp.get_core().new_disjunction(std::move(conjs));
    }

    void for_all_statement::execute(scope &scp, env &ctx) const
    {
        type *t = &scp.get_type(field_type.front().id);
        for (auto it = field_type.begin() + 1; it != field_type.end(); ++it)
            if (auto ct = dynamic_cast<complex_type *>(t))
                t = &ct->get_type(it->id);
            else
                throw std::runtime_error("cannot find type `" + it->id + "`..");

        if (auto ct = dynamic_cast<complex_type *>(t))
        {
            auto is = ct->get_instances();
            if (is.empty())
                throw inconsistency_exception();
            else
                for (auto ci : is)
                {
                    env c_ctx(ctx);
                    c_ctx.items.emplace(id.id, ci);
                    for (auto &stmnt : body)
                        stmnt->execute(scp, ctx);
                }
        }
        else
            throw std::runtime_error("cannot iterate over type `" + t->get_name() + "`..");
    }

    void conjunction_statement::execute(scope &scp, env &ctx) const
    {
        for (auto &stmnt : body)
            stmnt->execute(scp, ctx);
    }

    void formula_statement::execute(scope &scp, env &ctx) const
    {
        predicate *p;
        std::map<std::string, expr> assgnments;
        if (!formula_scope.empty())
        { // the formula's scope is explicitely declared..
            auto e = ctx.get(formula_scope.front().id);
            for (auto it = formula_scope.begin() + 1; it != formula_scope.end(); ++it)
                if (auto c_env = dynamic_cast<env *>(e.operator->()))
                    e = c_env->get(it->id);
                else
                    throw std::runtime_error("cannot find `" + it->id + "` item..");

            p = &static_cast<complex_type &>(e->get_type()).get_predicate(predicate_name.id);
            assgnments.emplace(TAU_KW, e);
        }
        else
        { // we inherit the formula's scope..
            p = &scp.get_predicate(predicate_name.id);
            if (!is_core(p->scope::get_scope()))
                assgnments.emplace(TAU_KW, ctx.get(TAU_KW));
        }

        for (size_t i = 0; i < assignment_names.size(); ++i)
        {
            auto v = assignment_values[i]->evaluate(scp, ctx);
            const auto &t = p->get_field(assignment_names[i].id).get_type();
            if (t.is_assignable_from(v->get_type())) // the target type is a superclass of the assignment..
                assgnments.emplace(assignment_names[i].id, v);
            else if (v->get_type().is_assignable_from(t)) // the assignment is a superclass of the target type..
            {
                if (scp.get_core().is_enum(v))
                { // the assignment is an enum (we prune the unassignable values)..
                    for (auto &val : scp.get_core().domain(v))
                        if (!t.is_assignable_from(val->get_type()))
                            scp.get_core().prune(v, val);
                }
                else // the assignment is a constant (which cannot be assigned to the target type)..
                    throw inconsistency_exception();
                assgnments.emplace(assignment_names[i].id, v);
            }
            else
                throw std::runtime_error("unrelated types");
        }

        auto atm_xpr = is_fact ? p->new_fact() : p->new_goal();
        auto &atm = static_cast<atom &>(*atm_xpr);
        atm.items.insert(assgnments.begin(), assgnments.end());

        // we initialize the unassigned atom's fields..
        std::queue<predicate *> q;
        q.push(p);
        while (!q.empty())
        {
            for (const auto &arg : q.front()->get_args())
                if (auto arg_it = atm.items.find(arg.get().get_name()); arg_it == atm.items.end())
                {
                    type &tp = arg.get().get_type();
                    if (tp.is_primitive())
                        atm.items.emplace(arg.get().get_name(), tp.new_instance());
                    else if (auto ct = dynamic_cast<complex_type *>(&tp))
                        switch (ct->get_instances().size())
                        {
                        case 0:
                            throw inconsistency_exception();
                        case 1:
                            atm.items.emplace(arg.get().get_name(), ct->get_instances().front());
                            break;
                        default:
                            atm.items.emplace(arg.get().get_name(), scp.get_core().new_enum(*ct, ct->get_instances()));
                        }
                    else
                        throw std::runtime_error("cannot find type");
                }
            for (const auto &sp : q.front()->get_parents())
                q.push(&sp.get());
            q.pop();
        }

        ctx.items.emplace(formula_name.id, atm_xpr);
    }

    void return_statement::execute(scope &scp, env &ctx) const { ctx.items.emplace(RETURN_KW, xpr->evaluate(scp, ctx)); }

    void method_declaration::refine(scope &scp) const
    {
        type *rt = nullptr; // the method's return type..
        if (!return_type.empty())
        {
            rt = &scp.get_type(return_type.front().id);
            for (auto it = return_type.begin() + 1; it != return_type.end(); ++it)
                if (auto ci = dynamic_cast<complex_type *>(rt))
                    rt = &ci->get_type(it->id);
                else
                    throw std::runtime_error("cannot find type");
        }

        std::vector<field_ptr> args; // the method's arguments..
        for (const auto &[tp_id_tkns, id_tkn] : parameters)
        {
            auto *tp = &scp.get_type(tp_id_tkns.front().id);
            for (auto it = tp_id_tkns.begin() + 1; it != tp_id_tkns.end(); ++it)
                if (auto ci = dynamic_cast<complex_type *>(tp))
                    tp = &ci->get_type(it->id);
                else
                    throw std::runtime_error("cannot find type");
            args.emplace_back(new field(*tp, id_tkn.id));
        }

        // we create the method and add it to the scope..
        auto mtd = new method(scp, name.id, std::move(args), body, rt);
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_method(std::move(mtd)); // we add the method to the core..
        else if (auto ct = dynamic_cast<complex_type *>(&scp))
            ct->add_method(std::move(mtd)); // we add the method to the complex type..
        else
            throw std::runtime_error("cannot add method");
    }

    void predicate_declaration::declare(scope &scp) const
    { // we create the predicate and add it to the scope..
        auto p = new predicate(scp, name.id, std::vector<field_ptr>(), body);
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_predicate(std::move(p)); // we add the predicate to the core..
        else if (auto ct = dynamic_cast<complex_type *>(&scp))
            ct->add_predicate(std::move(p)); // we add the predicate to the complex type..
        else
            throw std::runtime_error("cannot add predicate");
    }

    void predicate_declaration::refine(scope &scp) const
    {
        auto &p = scp.get_predicate(name.id); // the predicate to refine..

        // the predicate's arguments..
        for (const auto &[tp_id_tkns, id_tkn] : parameters)
        { // we find the type..
            auto *tp = &scp.get_type(tp_id_tkns.front().id);
            for (auto it = tp_id_tkns.begin() + 1; it != tp_id_tkns.end(); ++it)
                if (auto ci = dynamic_cast<complex_type *>(tp))
                    tp = &ci->get_type(it->id);
                else
                    throw std::runtime_error("cannot find type");

            // we add the argument to the predicate..
            p.add_arg(new field(*tp, id_tkn.id));
        }

        // the predicate's parents..
        for (const auto &sp : predicate_list)
        {
            auto sp_p = &scp.get_predicate(sp.front().id);
            for (auto it = sp.begin() + 1; it != sp.end(); ++it)
                sp_p = &sp_p->get_predicate(it->id);
            p.add_parent(*sp_p);
        }
    }

    void typedef_declaration::declare(scope &scp) const
    { // we create the typedef and add it to the scope..
        auto td = new typedef_type(scp, name.id, scp.get_type(primitive_type.id), xpr);
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(td); // we add the typedef to the core..
        else if (auto ct = dynamic_cast<complex_type *>(&scp))
            ct->add_type(td); // we add the typedef to the complex type..
        else
            throw std::runtime_error("cannot add typedef");
    }

    void enum_declaration::declare(scope &scp) const
    { // we create the enum and add it to the scope..
        auto en = new enum_type(scp, name.id);

        // the enum's instances..
        for (const auto &e : enums)
            en->instances.emplace_back(scp.get_core().new_string(e.str));

        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(en); // we add the enum to the core..
        else if (auto ct = dynamic_cast<complex_type *>(&scp))
            ct->add_type(en); // we add the enum to the complex type..
        else
            throw std::runtime_error("cannot add enum");
    }

    void enum_declaration::refine(scope &scp) const
    {
        if (!type_refs.empty())
        {
            if (auto en = dynamic_cast<enum_type *>(&scp.get_type(name.id))) // the enum to refine..
                for (const auto &tr : type_refs)
                {
                    auto *tp = &scp.get_type(tr.front().id);
                    for (auto it = tr.begin() + 1; it != tr.end(); ++it)
                        if (auto ci = dynamic_cast<complex_type *>(tp))
                            tp = &ci->get_type(it->id);
                        else
                            throw std::runtime_error("cannot find type");
                    if (auto e = dynamic_cast<enum_type *>(tp))
                        en->enums.emplace_back(*e);
                    else
                        throw std::runtime_error("cannot refine enum with non-enum type");
                }
            else
                throw std::runtime_error("cannot refine non-enum type");
        }
    }

    void field_declaration::refine(scope &scp) const
    {
        auto *tp = &scp.get_type(field_type.front().id);
        for (auto it = field_type.begin() + 1; it != field_type.end(); ++it)
            if (auto ci = dynamic_cast<complex_type *>(tp))
                tp = &ci->get_type(it->id);
            else
                throw std::runtime_error("cannot find type");

        for (const auto &d : declarations)
        {
            auto *fld = new field(*tp, d->name.id, d->xpr);
            if (auto cr = dynamic_cast<core *>(&scp))
                cr->add_field(fld); // we add the field to the core..
            else if (auto ct = dynamic_cast<complex_type *>(&scp))
                ct->add_field(fld); // we add the field to the complex type..
            else
                throw std::runtime_error("cannot add field");
        }
    }

    void constructor_declaration::refine(scope &scp) const
    {
        std::vector<field_ptr> args;
        args.reserve(parameters.size());
        for (const auto &[tp_id_tkns, id_tkn] : parameters)
        { // we find the type..
            auto *tp = &scp.get_type(tp_id_tkns.front().id);
            for (auto it = tp_id_tkns.begin() + 1; it != tp_id_tkns.end(); ++it)
                if (auto ci = dynamic_cast<complex_type *>(tp))
                    tp = &ci->get_type(it->id);
                else
                    throw std::runtime_error("cannot find type");

            // we add the argument to the constructor..
            args.emplace_back(new field(*tp, id_tkn.id));
        }

        if (auto c = dynamic_cast<complex_type *>(&scp))
            c->add_constructor(new constructor(*c, std::move(args), init_names, init_vals, body)); // we add the constructor to the complex type..
        else
            throw std::runtime_error("cannot add constructor");
    }

    void class_declaration::declare(scope &scp) const
    { // we create the class and add it to the scope..
        auto cl = new complex_type(scp, name.id);
        if (auto cr = dynamic_cast<core *>(&scp))
            cr->add_type(cl); // we add the class to the core..
        else if (auto ct = dynamic_cast<complex_type *>(&scp))
            ct->add_type(cl); // we add the class to the complex type..
        else
            throw std::runtime_error("cannot add class");

        // we declare the enclosed types..
        for (const auto &t : types)
            t->declare(*cl);
    }

    void class_declaration::refine(scope &scp) const
    {
        auto &tp = scp.get_type(name.id);
        if (auto c_tp = dynamic_cast<complex_type *>(&tp))
        { // we add the base classes..
            for (const auto &t : base_classes)
            { // we find the base class..
                auto *bc = &scp.get_type(t.front().id);
                for (auto it = t.begin() + 1; it != t.end(); ++it)
                    if (auto ci = dynamic_cast<complex_type *>(bc))
                        bc = &ci->get_type(it->id);
                    else
                        throw std::runtime_error("cannot find type");

                if (auto bc_ct = dynamic_cast<complex_type *>(bc))
                    c_tp->parents.emplace_back(*bc_ct); // we add the base class to the class..
                else
                    throw std::runtime_error("cannot add non-class type as base class");
            }

            // we refine the fields..
            for (const auto &f : fields)
                f->refine(*c_tp);
            // we refine the constructors..
            for (const auto &c : constructors)
                c->refine(*c_tp);
            // we refine the methods..
            for (const auto &m : methods)
                m->refine(*c_tp);
            // we refine the enclosed types..
            for (const auto &t : types)
                t->refine(*c_tp);
            // we declare the enclosed predicates..
            for (const auto &p : predicates)
                p->declare(*c_tp);
        }
        else
            throw std::runtime_error("cannot refine non-class type");
    }

    void class_declaration::refine_predicates(scope &scp) const
    {
        auto &tp = scp.get_type(name.id);
        if (auto c_tp = dynamic_cast<complex_type *>(&tp))
        { // we refine the enclosed predicates..
            for (const auto &p : predicates)
                p->refine(*c_tp);
            // we refine the enclosed types..
            for (const auto &t : types)
                t->refine_predicates(*c_tp);
        }
        else
            throw std::runtime_error("cannot refine non-class type");
    }

    void compilation_unit::declare(scope &scp) const
    {
        // we declare the types..
        for (const auto &t : types)
            t->declare(scp);
        // we declare the predicates..
        for (const auto &p : predicates)
            p->declare(scp);
    }

    void compilation_unit::refine(scope &scp) const
    {
        // we refine the types..
        for (const auto &t : types)
            t->refine(scp);
        // we refine the methods..
        for (const auto &m : methods)
            m->refine(scp);
        // we refine the predicates..
        for (const auto &p : predicates)
            p->refine(scp);
    }

    void compilation_unit::refine_predicates(scope &scp) const
    {
        // we refine the types..
        for (const auto &t : types)
            t->refine_predicates(scp);
    }

    void compilation_unit::execute(scope &scp, env &ctx) const
    {
        // we execute the statements..
        for (const auto &s : body)
            s->execute(scp, ctx);
    }

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
        return tks[pos++].operator->();
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
        tk = tks[pos - 1].operator->();
    }

    RIDDLE_EXPORT compilation_unit_ptr parser::parse()
    {
        tk = next();
        return _compilation_unit();
    }

    compilation_unit_ptr parser::_compilation_unit()
    {
        std::vector<type_declaration_ptr> ts;
        std::vector<method_declaration_ptr> ms;
        std::vector<predicate_declaration_ptr> ps;
        std::vector<statement_ptr> stmnts;

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

        return new compilation_unit(std::move(ms), std::move(ps), std::move(ts), std::move(stmnts));
    }

    const typedef_declaration *parser::_typedef_declaration()
    {
        id_token *pt = nullptr;
        expression_ptr e;

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
            pt = new id_token(0, 0, 0, 0, TIME_POINT_KW);
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
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

        if (!match(SEMICOLON_ID))
            error("expected `;`..");

        return new typedef_declaration(n, *pt, std::move(e));
    }

    const enum_declaration *parser::_enum_declaration()
    {
        std::vector<string_token> es;
        std::vector<std::vector<id_token>> trs;

        if (!match(ENUM_ID))
            error("expected `enum`..");

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

        do
        {
            switch (tk->sym)
            {
            case LBRACE_ID:
                tk = next();
                if (!match(StringLiteral_ID))
                    error("expected string literal..");
                es.emplace_back(*static_cast<const string_token *>(tks[pos - 2].operator->()));

                while (match(COMMA_ID))
                {
                    if (!match(StringLiteral_ID))
                        error("expected string literal..");
                    es.emplace_back(*static_cast<const string_token *>(tks[pos - 2].operator->()));
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
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
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

        return new enum_declaration(n, std::move(es), std::move(trs));
    }

    const class_declaration *parser::_class_declaration()
    {
        std::vector<std::vector<id_token>> bcs;      // the base classes..
        std::vector<field_declaration_ptr> fs;       // the fields of the class..
        std::vector<constructor_declaration_ptr> cs; // the constructors of the class..
        std::vector<method_declaration_ptr> ms;      // the methods of the class..
        std::vector<predicate_declaration_ptr> ps;   // the predicates of the class..
        std::vector<type_declaration_ptr> ts;        // the types of the class..

        if (!match(CLASS_ID))
            error("expected `class`..");

        if (!match(ID_ID))
            error("expected identifier..");
        // the name of the class..
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

        if (match(COLON_ID))
        {
            do
            {
                std::vector<id_token> ids;
                do
                {
                    if (!match(ID_ID))
                        error("expected identifier..");
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
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
            cs.emplace_back(new constructor_declaration({}, {}, {}, {}));

        return new class_declaration(n, std::move(bcs), std::move(fs), std::move(cs), std::move(ms), std::move(ps), std::move(ts));
    }

    field_declaration_ptr parser::_field_declaration()
    {
        std::vector<id_token> tp;
        std::vector<variable_declaration_ptr> ds;

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
            tp.emplace_back(id_token(0, 0, 0, 0, TIME_POINT_KW));
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
                tp.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            }
            break;
        default:
            error("expected either `bool` or `int` or `real` or `string` or an identifier..");
        }

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

        if (match(EQ_ID))
            ds.emplace_back(new variable_declaration(n, _expression()));
        else
            ds.emplace_back(new variable_declaration(n));

        while (match(COMMA_ID))
        {
            if (!match(ID_ID))
                error("expected identifier..");
            auto c_n = *static_cast<const id_token *>(tks[pos - 2].operator->());

            if (match(EQ_ID))
                ds.emplace_back(new variable_declaration(c_n, _expression()));
            else
                ds.emplace_back(new variable_declaration(c_n));
        }

        if (!match(SEMICOLON_ID))
            error("expected `;`..");

        return new field_declaration(std::move(tp), std::move(ds));
    }

    method_declaration_ptr parser::_method_declaration()
    {
        std::vector<id_token> rt;
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<statement_ptr> stmnts;

        if (!match(VOID_ID))
        {
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                rt.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            } while (match(DOT_ID));
        }

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

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
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_POINT_KW));
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
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
                    }
                    break;
                default:
                    error("expected either `bool` or `int` or `real` or `string` or an identifier..");
                }
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].operator->());
                pars.emplace_back(p_ids, pn);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("expected `)`..");
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
            stmnts.emplace_back(_statement());

        return new method_declaration(std::move(rt), n, std::move(pars), std::move(stmnts));
    }

    constructor_declaration_ptr parser::_constructor_declaration()
    {
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<id_token> ins;
        std::vector<std::vector<expression_ptr>> ivs;
        std::vector<statement_ptr> stmnts;

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
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
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
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_POINT_KW));
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
                auto pn = *static_cast<const id_token *>(tks[pos - 2].operator->());
                pars.emplace_back(p_ids, pn);
            } while (match(COMMA_ID));

            if (!match(RPAREN_ID))
                error("expected `)`..");
        }

        if (match(COLON_ID))
        {
            do
            {
                std::vector<expression_ptr> xprs;
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].operator->());

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

        return new constructor_declaration(pars, std::move(ins), std::move(ivs), std::move(stmnts));
    }

    predicate_declaration_ptr parser::_predicate_declaration()
    {
        std::vector<std::pair<const std::vector<id_token>, const id_token>> pars;
        std::vector<std::vector<id_token>> pl;
        std::vector<statement_ptr> stmnts;

        if (!match(PREDICATE_ID))
            error("expected `predicate`..");

        if (!match(ID_ID))
            error("expected identifier..");
        auto n = *static_cast<const id_token *>(tks[pos - 2].operator->());

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
                    p_ids.emplace_back(id_token(0, 0, 0, 0, TIME_POINT_KW));
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
                        p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
                    }
                    break;
                default:
                    error("expected either `bool` or `int` or `real` or `string` or an identifier..");
                }
                if (!match(ID_ID))
                    error("expected identifier..");
                auto pn = *static_cast<const id_token *>(tks[pos - 2].operator->());
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
                    p_ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
                } while (match(DOT_ID));
                pl.emplace_back(std::move(p_ids));
            } while (match(COMMA_ID));
        }

        if (!match(LBRACE_ID))
            error("expected `{`..");

        while (!match(RBRACE_ID))
            stmnts.emplace_back(_statement());

        return new predicate_declaration(n, std::move(pars), std::move(pl), std::move(stmnts));
    }

    statement_ptr parser::_statement()
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
                ft.emplace_back(id_token(0, 0, 0, 0, TIME_POINT_KW));
                break;
            case STRING_ID:
                ft.emplace_back(id_token(0, 0, 0, 0, STRING_KW));
                break;
            default:
                error("expected either `bool` or `int` or `real` or `string`..");
            }
            tk = next();

            std::vector<id_token> ns;
            std::vector<expression_ptr> es;

            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                ns.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));

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

            return new local_field_statement(std::move(ft), std::move(ns), std::move(es));
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
                is.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            }

            switch (tk->sym)
            {
            case ID_ID: // a local field..
            {
                std::vector<id_token> ns;
                std::vector<expression_ptr> es;

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

                return new local_field_statement(std::move(is), std::move(ns), std::move(es));
            }
            case EQ_ID: // an assignment..
            {
                id_token i = is.back();
                is.pop_back();
                tk = next();
                auto e = _expression();
                if (!match(SEMICOLON_ID))
                    error("expected `;`..");
                return new assignment_statement(std::move(is), i, std::move(e));
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
                auto e = _expression();
                if (!match(SEMICOLON_ID))
                    error("expected `;`..");
                return new expression_statement(std::move(e));
            }
            default:
                error("expected either `=` or an identifier..");
                return nullptr;
            }
        }
        case LBRACE_ID: // either a block or a disjunction..
        {
            tk = next();
            std::vector<statement_ptr> stmnts;
            while (!match(RBRACE_ID))
                stmnts.emplace_back(_statement());
            switch (tk->sym)
            {
            case LBRACKET_ID:
            case OR_ID: // a disjunctive statement..
            {
                std::vector<std::vector<statement_ptr>> conjs;
                std::vector<expression_ptr> conj_costs;
                expression_ptr e = nullptr;
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
                return new disjunction_statement(std::move(conjs), std::move(conj_costs));
            }
            default: // a conjunction statement..
                return new conjunction_statement(std::move(stmnts));
            }
        }
        case FACT_ID:
        case GOAL_ID:
        {
            bool isf = tk->sym == FACT_ID;
            tk = next();
            std::vector<id_token> scp;
            std::vector<id_token> assn_ns;
            std::vector<expression_ptr> assn_vs;

            if (!match(ID_ID))
                error("expected identifier..");
            auto fn = *static_cast<const id_token *>(tks[pos - 2].operator->());

            if (!match(EQ_ID))
                error("expected `=`..");

            if (!match(NEW_ID))
                error("expected `new`..");

            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                scp.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
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
                    id_token assgn_name = *static_cast<const id_token *>(tks[pos - 2].operator->());

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
            return new formula_statement(isf, fn, scp, pn, assn_ns, std::move(assn_vs));
        }
        case FOR_ID:
        {
            tk = next();
            std::vector<id_token> ft;
            id_token *id = nullptr;
            std::vector<statement_ptr> stmnts;

            if (!match(LPAREN_ID))
                error("expected `(`..");
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                ft.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            } while (match(DOT_ID));

            if (!match(ID_ID))
                error("expected identifier..");
            id = new id_token(*static_cast<const id_token *>(tks[pos - 2].operator->()));

            if (!match(RPAREN_ID))
                error("expected `)`..");

            if (!match(LBRACE_ID))
                error("expected `{`..");
            while (!match(RBRACE_ID))
                stmnts.emplace_back(_statement());
            return new for_all_statement(ft, *id, std::move(stmnts));
        }
        case RETURN_ID:
        {
            auto e = _expression();
            if (!match(SEMICOLON_ID))
                error("expected `;`..");
            return new return_statement(std::move(e));
        }
        default:
        {
            auto xpr = _expression();
            if (!match(SEMICOLON_ID))
                error("expected `;`..");
            return new expression_statement(std::move(xpr));
        }
        }
    }

    expression_ptr parser::_expression(const size_t &pr)
    {
        expression_ptr e = nullptr;
        switch (tk->sym)
        {
        case BoolLiteral_ID:
            tk = next();
            e = new bool_literal_expression(*static_cast<const bool_token *>(tks[pos - 2].operator->()));
            break;
        case IntLiteral_ID:
            tk = next();
            e = new int_literal_expression(*static_cast<const int_token *>(tks[pos - 2].operator->()));
            break;
        case RealLiteral_ID:
            tk = next();
            e = new real_literal_expression(*static_cast<const real_token *>(tks[pos - 2].operator->()));
            break;
        case StringLiteral_ID:
            tk = next();
            e = new string_literal_expression(*static_cast<const string_token *>(tks[pos - 2].operator->()));
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
                    ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
                } while (match(DOT_ID));

                if (!match(RPAREN_ID))
                    error("expected `)`..");
                e = new cast_expression(std::move(ids), _expression());
            }
            else // a parenthesis..
            {
                backtrack(c_pos);
                e = _expression();
                if (!match(RPAREN_ID))
                    error("expected `)`..");
            }
            break;
        }
        case PLUS_ID:
            tk = next();
            e = new plus_expression(_expression(4));
            break;
        case MINUS_ID:
            tk = next();
            e = new minus_expression(_expression(4));
            break;
        case BANG_ID:
            tk = next();
            e = new not_expression(_expression(4));
            break;
        case NEW_ID:
        {
            tk = next();
            std::vector<id_token> ids;
            do
            {
                if (!match(ID_ID))
                    error("expected identifier..");
                ids.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            } while (match(DOT_ID));

            std::vector<expression_ptr> xprs;
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

            e = new constructor_expression(std::move(ids), std::move(xprs));
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
                is.emplace_back(*static_cast<const id_token *>(tks[pos - 2].operator->()));
            }
            if (match(LPAREN_ID))
            {
                tk = next();
                id_token fn = is.back();
                is.pop_back();
                std::vector<expression_ptr> xprs;
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

                e = new function_expression(std::move(is), fn, std::move(xprs));
            }
            else
                e = new id_expression(std::move(is));
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
                e = new eq_expression(std::move(e), _expression(1));
                break;
            case BANGEQ_ID:
                assert(0 >= pr);
                tk = next();
                e = new neq_expression(std::move(e), _expression(1));
                break;
            case LT_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new lt_expression(std::move(e), _expression(1));
                break;
            }
            case LTEQ_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new leq_expression(std::move(e), _expression(1));
                break;
            }
            case GTEQ_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new geq_expression(std::move(e), _expression(1));
                break;
            }
            case GT_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new gt_expression(std::move(e), _expression(1));
                break;
            }
            case IMPLICATION_ID:
            {
                assert(1 >= pr);
                tk = next();
                e = new implication_expression(std::move(e), _expression(1));
                break;
            }
            case BAR_ID:
            {
                assert(1 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(BAR_ID))
                    xprs.emplace_back(_expression(2));

                e = new disjunction_expression(std::move(xprs));
                break;
            }
            case AMP_ID:
            {
                assert(1 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(AMP_ID))
                    xprs.emplace_back(_expression(2));

                e = new conjunction_expression(std::move(xprs));
                break;
            }
            case CARET_ID:
            {
                assert(1 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(CARET_ID))
                    xprs.emplace_back(_expression(2));

                e = new exct_one_expression(std::move(xprs));
                break;
            }
            case PLUS_ID:
            {
                assert(2 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(PLUS_ID))
                    xprs.emplace_back(_expression(3));

                e = new addition_expression(std::move(xprs));
                break;
            }
            case MINUS_ID:
            {
                assert(2 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(MINUS_ID))
                    xprs.emplace_back(_expression(3));

                e = new subtraction_expression(std::move(xprs));
                break;
            }
            case STAR_ID:
            {
                assert(3 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(STAR_ID))
                    xprs.emplace_back(_expression(4));

                e = new multiplication_expression(std::move(xprs));
                break;
            }
            case SLASH_ID:
            {
                assert(3 >= pr);
                std::vector<expression_ptr> xprs;
                xprs.emplace_back(std::move(e));

                while (match(SLASH_ID))
                    xprs.emplace_back(_expression(4));

                e = new division_expression(std::move(xprs));
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