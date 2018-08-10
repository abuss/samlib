#pragma once

#include <samlib/agent/agent.hpp>

namespace samlib
{

template <typename GS, typename Tin, typename Tout = Tin>
class transform_agent
  : public agent<GS, Tin, Tout>
{
  typedef agent<GS, Tin, Tout>      base_t;
  typedef std::function<Tout(Tin)>  task_t;

  task_t task;

protected:

  using base_t::global_state;
  using base_t::outputs;

public:

  using typename base_t::agent;

  transform_agent(GS &gstate, task_t&& fn)
      : base_t(gstate),
        task(fn)
  { }

  void run()
  {
    while (!global_state->terminate)
    {
      if (auto dat = this->try_receive())
        outputs.template send_to<0>(task(std::move(*dat)));
        // std::get<0>(outputs)->send(task(std::move(*dat)));
    }
  }
};

} // namespace samlib
