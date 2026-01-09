#pragma once

#include "rational.hpp"
#include "json.hpp"
#include "core.hpp"
#include <vector>
#include <cstdint>
#include <memory>

namespace riddle
{
  class flaw;
  class resolver;

  class graph : public core
  {
  public:
    graph(std::string_view name = "RiDDLe") noexcept;

    /**
     * @brief Creates a new flaw of the given type.
     *
     * @tparam Tp The type of the flaw to create.
     * @tparam Args The types of the arguments to pass to the flaw
     * @param args The arguments to pass to the flaw
     * @return Tp& The created flaw
     */
    template <typename Tp, typename... Args>
    Tp &new_flaw(Args &&...args) noexcept
    {
      static_assert(std::is_base_of_v<flaw, Tp>, "Tp must be a subclass of flaw");
      auto f = std::make_unique<Tp>(std::forward<Args>(args)...);
      Tp &ref = *f;
      flaws.emplace_back(std::move(f));
#ifdef RIDDLE_ENABLE_LISTENERS
      flaw_created(ref);
#endif
      return ref;
    }

    /**
     * @brief Creates a new resolver of the given type.
     *
     * @tparam Tp The type of the resolver to create.
     * @tparam Args The types of the arguments to pass to the resolver
     * @param args The arguments to pass to the resolver
     * @return Tp& The created resolver
     */
    template <typename Tp, typename... Args>
    Tp &new_resolver(Args &&...args) noexcept
    {
      static_assert(std::is_base_of_v<resolver, Tp>, "Tp must be a subclass of resolver");
      auto r = std::make_unique<Tp>(std::forward<Args>(args)...);
      r->flw.resolvers.emplace_back(*r); // register the resolver in its flaw..
      Tp &ref = *r;
      resolvers.emplace_back(std::move(r));
#ifdef RIDDLE_ENABLE_LISTENERS
      resolver_created(ref);
#endif
      return ref;
    }

    /**
     * @brief Adds a causal link between a flaw and a resolver.
     *
     * This function establishes a causal relationship between the specified flaw and resolver.
     *
     * @param f The flaw to which the causal link is to be added.
     * @param r The resolver that is the cause of the flaw.
     */
    virtual void add_causal_link(flaw &f, resolver &r) noexcept;

    // [[nodiscard]] std::vector<std::unique_ptr<flaw>> &get_flaws() noexcept { return flaws; }
    [[nodiscard]] const std::vector<std::unique_ptr<flaw>> &get_flaws() const noexcept { return flaws; }
    [[nodiscard]] const std::optional<std::reference_wrapper<flaw>> &get_current_flaw() const noexcept { return c_flaw; }
    [[nodiscard]] const std::vector<std::unique_ptr<resolver>> &get_resolvers() const noexcept { return resolvers; }
    [[nodiscard]] const std::optional<std::reference_wrapper<resolver>> &get_current_resolver() const noexcept { return c_res; }

    [[nodiscard]] virtual json::json to_json() const override;

  protected:
    void compute_resolvers(flaw &flw);
    bool apply_resolver(resolver &res, bool temp_res = false) noexcept;

    void set_current_flaw(std::optional<std::reference_wrapper<flaw>> flw) noexcept;
    void set_current_resolver(std::optional<std::reference_wrapper<resolver>> res) noexcept;

    void set_flaw_cost(flaw &flw, utils::rational &cost) noexcept;

#ifdef RIDDLE_ENABLE_LISTENERS
  private:
    /**
     * @brief Notifies when a flaw has been created.
     *
     * This function is called when a flaw has been created. It is a virtual function that can be overridden by derived classes to perform specific actions when a flaw is created.
     *
     * @param flaw The flaw that has been created.
     */
    virtual void flaw_created(const flaw &) {}
    /**
     * @brief Notifies when the cost of a flaw has changed.
     *
     * This function is called when the cost of a flaw has changed. It is a virtual function that can be overridden by derived classes to perform specific actions when a flaw's cost changes.
     *
     * @param flaw The flaw whose cost has changed.
     */
    virtual void flaw_cost_changed(const flaw &) {}
    /**
     * @brief Notifies when the current flaw has changed.
     *
     * This function is called when the current flaw has changed. It is a virtual function that can be overridden by derived classes to perform specific actions when the current flaw changes.
     *
     * @param flaw The current flaw.
     */
    virtual void current_flaw(std::optional<std::reference_wrapper<flaw>>) {}

    /**
     * @brief Notifies when a resolver has been created.
     *
     * This function is called when a resolver has been created. It is a virtual function that can be overridden by derived classes to perform specific actions when a resolver is created.
     *
     * @param resolver The resolver that has been created.
     */
    virtual void resolver_created(const resolver &) {}
    /**
     * @brief Notifies when the current resolver has changed.
     *
     * This function is called when the current resolver has changed. It is a virtual function that can be overridden by derived classes to perform specific actions when the current resolver changes.
     *
     * @param resolver The current resolver.
     */
    virtual void current_resolver(std::optional<std::reference_wrapper<resolver>>) {}

    /**
     * @brief Notifies when a causal link has been added.
     *
     * This function is called when a causal link has been added. It is a virtual function that can be overridden by derived classes to perform specific actions when a causal link is added.
     *
     * @param flaw The flaw that is the source of the causal link.
     * @param resolver The resolver that is the destination of the causal link.
     */
    virtual void causal_link_added(const flaw &, const resolver &) {}
#endif

  private:
    std::vector<std::unique_ptr<flaw>> flaws;              // The set of flaws
    std::vector<std::unique_ptr<resolver>> resolvers;      // The set of resolvers
    std::optional<std::reference_wrapper<flaw>> c_flaw;    // The current flaw..
    std::optional<std::reference_wrapper<resolver>> c_res; // The current resolver..
  };

  class flaw
  {
    friend class graph;

  public:
    flaw(graph &gr, std::optional<std::reference_wrapper<resolver>> cause) : flaw(gr, cause.has_value() ? std::vector<std::reference_wrapper<resolver>>{cause.value()} : std::vector<std::reference_wrapper<resolver>>{}) {}
    flaw(graph &gr, std::vector<std::reference_wrapper<resolver>> &&causes);
    flaw(const flaw &) = delete;
    virtual ~flaw() = default;

    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    [[nodiscard]] graph &get_graph() const noexcept { return gr; }
    [[nodiscard]] const utils::rational &get_estimated_cost() const noexcept { return est_cost; }

    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_causes() const noexcept { return causes; }
    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_supports() const noexcept { return supports; }
    [[nodiscard]] const std::vector<std::reference_wrapper<resolver>> &get_resolvers() const noexcept { return resolvers; }

    [[nodiscard]] virtual json::json to_json() const;

  protected:
    template <typename Tp, typename... Args>
    Tp &new_resolver(Args &&...args) noexcept { return gr.new_resolver<Tp>(std::forward<Args>(args)...); }

    void add_support(resolver &res) noexcept;

  private:
    virtual void compute_resolvers() = 0;
    friend bool have_common_ancestors(const flaw &a, const flaw &b);

  protected:
    graph &gr; // the graph this flaw belongs to..

  private:
    utils::rational est_cost = utils::rational::positive_infinite; // the estimated cost of this flaw..
    std::vector<std::reference_wrapper<resolver>> causes;          // the causes that led to this flaw..
    std::vector<std::reference_wrapper<resolver>> supports;        // the resolvers supported by this flaw..
    std::vector<std::reference_wrapper<resolver>> resolvers;       // the resolvers for this flaw..
  };

  class resolver
  {
    friend class graph;
    friend class flaw;

  public:
    resolver(flaw &flw, utils::rational &&intrinsic_cost);
    resolver(const resolver &) = delete;
    virtual ~resolver() = default;

    [[nodiscard]] uintptr_t get_id() const noexcept { return reinterpret_cast<uintptr_t>(this); }

    [[nodiscard]] flaw &get_flaw() const noexcept { return flw; }

    [[nodiscard]] const utils::rational &get_intrinsic_cost() const noexcept { return intrinsic_cost; }
    [[nodiscard]] const std::vector<std::reference_wrapper<flaw>> &get_preconditions() const noexcept { return preconditions; }

    [[nodiscard]] utils::rational get_estimated_cost() const noexcept;

    [[nodiscard]] virtual json::json to_json() const;

  private:
    [[nodiscard]] virtual bool apply() noexcept = 0;

  protected:
    flaw &flw; // the flaw solved by this resolver..

  private:
    const utils::rational intrinsic_cost;                    // The intrinsic cost of this resolver..
    std::vector<std::reference_wrapper<flaw>> preconditions; // The preconditions of this resolver..
  };

  /**
   * @brief Determines whether two flaws share any common ancestors.
   *
   * This function checks if the given flaws `a` and `b` have any ancestors in common
   * within the flaw hierarchy. It can be used to identify relationships or dependencies
   * between flaws based on their ancestry.
   *
   * @param a The first flaw to compare.
   * @param b The second flaw to compare.
   * @return true if `a` and `b` have at least one common ancestor; false otherwise.
   */
  [[nodiscard]] bool have_common_ancestors(const flaw &a, const flaw &b);
} // namespace riddle
