#pragma once

#include "core.h"

namespace riddle
{
  class core_listener
  {
    friend class core;

  public:
    core_listener(core &cr) : cr(cr) { cr.listeners.emplace_back(this); }
    virtual ~core_listener() { cr.listeners.erase(std::find(cr.listeners.begin(), cr.listeners.end(), this)); }

  private:
    virtual void log(const std::string &) {}
    virtual void read(const std::string &) {}
    virtual void read(const std::vector<std::string> &) {}

    virtual void state_changed() {}

    virtual void started_solving() {}
    virtual void solution_found() {}
    virtual void inconsistent_problem() {}

  protected:
    core &cr;
  };
} // namespace riddle
