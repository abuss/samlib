#pragma once

#include <samlib/agent/agent.hpp>

namespace samlib
{

template <typename GS, typename Tin, typename Tout = Tin>
struct transform_agent
  : public samlib::agent<GS, Tin, Tout>
{
  typedef samlib::agent<GS, Tin, Tout> base_t;
  typedef std::function<Tout(Tin)> Task;

  using base_t::global_state;
  using base_t::outputs;
  using typename base_t::agent;

  Task task;

  transform_agent(GS &gstate, Task fn)
      : base_t(gstate),
        task(fn)
  {
  }

  void run()
  {
    while (!global_state->terminate)
    {
      if (auto dat = this->try_receive())
      {
        std::get<0>(outputs)->send(task(std::move(*dat)));
      }
    }
  }
};

} // namespace samlib
