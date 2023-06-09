#include "constructor.h"
#include "type.h"
#include "item.h"
#include "core.h"
#include <cassert>

namespace riddle
{
    RIDDLE_EXPORT constructor::constructor(complex_type &tp, std::vector<field_ptr> as, const std::vector<riddle::id_token> &ins, const std::vector<std::vector<ast::expression_ptr>> &ivs, const std::vector<ast::statement_ptr> &body) : scope(tp), init_names(ins), init_vals(ivs), body(body)
    { // we add the arguments..
        args.reserve(as.size());
        for (auto &arg : as)
        {
            args.emplace_back(*arg);
            add_field(std::move(arg));
        }
    }

    RIDDLE_EXPORT void constructor::call(expr &self, std::vector<expr> exprs)
    { // we create a new environment for the constructor..
        env ctx(static_cast<complex_item *>(self.operator->()));
        for (size_t i = 0; i < args.size(); ++i)
        { // ..and we add the arguments to it
            if (!args[i].get().get_type().is_assignable_from(exprs[i]->get_type()))
                throw std::runtime_error("invalid argument type for `" + args[i].get().get_name() + "`");
            ctx.items.emplace(args[i].get().get_name(), std::move(exprs[i]));
        }

        // we manage the initialization list..
        for (size_t il_idx = 0; il_idx < init_names.size(); il_idx++)
            try
            { // we try to find the field in the current type..
                auto &f = get_field(init_names.at(il_idx).id);
                if (f.get_type().is_primitive())
                { // we evaluate the expression..
                    assert(init_vals[il_idx].size() == 1);
                    dynamic_cast<env &>(*self).items.emplace(init_names[il_idx].id, init_vals[il_idx][0]->evaluate(*this, ctx));
                }
                else
                { // we call the constructor..
                    std::vector<expr> c_exprs;
                    std::vector<std::reference_wrapper<type>> par_types;
                    for (const auto &ex : init_vals.at(il_idx))
                    {
                        expr c_expr = ex->evaluate(*this, ctx);
                        c_exprs.push_back(c_expr);
                        par_types.push_back(c_expr->get_type());
                    }

                    if (par_types.size() == 1 && f.get_type().is_assignable_from(par_types.begin()->get())) // we have a direct assignment..
                        dynamic_cast<env &>(*self).items.emplace(init_names[il_idx].id, std::move(*c_exprs.begin()));
                    else
                    { // we call the field's constructor..
                        auto &tp = static_cast<complex_type &>(f.get_type());
                        auto inst = tp.new_instance();
                        tp.get_constructor(par_types).call(inst, std::move(c_exprs));
                        dynamic_cast<env &>(*self).items.emplace(init_names[il_idx].id, std::move(inst));
                    }
                }
            }
            catch (const std::out_of_range &)
            { // there is no field in the current type with the given name, so we call the supertype's constructor..
                auto st = std::find_if(static_cast<complex_type &>(get_scope()).get_parents().begin(), static_cast<complex_type &>(get_scope()).get_parents().end(), [this, il_idx](auto &st)
                                       { return init_names.at(il_idx).id == st.get().get_name(); });
                if (st == static_cast<complex_type &>(get_scope()).get_parents().end())
                    throw std::runtime_error("invalid call to `" + init_names[il_idx].id + "` supertype constructor");
                std::vector<expr> c_exprs;
                std::vector<std::reference_wrapper<type>> par_types;
                for (const auto &ex : init_vals.at(il_idx))
                {
                    expr c_expr = ex->evaluate(*this, ctx);
                    c_exprs.push_back(c_expr);
                    par_types.push_back(c_expr->get_type());
                }

                // we assume that the constructor exists..
                (*st).get().get_constructor(par_types).call(self, std::move(c_exprs));
            }

        // we instantiate the uninstantiated fields..
        for (const auto &[f_name, f] : get_scope().get_fields())
            if (!f->is_synthetic() && !dynamic_cast<env &>(*self).items.count(f_name))
            { // the field is uninstantiated..
                if (f->get_expression())
                    dynamic_cast<env &>(*self).items.emplace(f_name, f->get_expression()->evaluate(*this, ctx));
                else
                {
                    type &tp = f->get_type();
                    if (tp.is_primitive())
                        dynamic_cast<env &>(*self).items.emplace(f_name, tp.new_instance());
                    else if (auto ctp = dynamic_cast<complex_type *>(&tp))
                        dynamic_cast<env &>(*self).items.emplace(f_name, get_core().new_enum(*ctp, ctp->get_instances()));
                    else
                        throw std::runtime_error("invalid field type");
                }
            }

        // we execute the constructor body..
        for (const auto &stmnt : body)
            stmnt->execute(*this, ctx);
    }
} // namespace riddle
