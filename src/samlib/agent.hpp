#pragma once

#include <samlib/base.hpp>
#include <samlib/mailbox.hpp>

namespace samlib
{

struct empty_state {};

template <typename A>
class agent_ref;

template <typename GState, typename Tin>
class agent
  : public base<Tin>
{
protected:
  using base_t = base<Tin>;
  using global_state_t = GState;
  using task_t = std::function<void(global_state_t&, mailbox<Tin> &)>;

  global_state_t *global_state;
  task_t task;

public:
  using agent_ref_type = agent_ref<agent>;

  constexpr agent(global_state_t &gstate)
      : global_state{&gstate}
  { }

  constexpr agent(global_state_t &gstate, task_t &&fn)
      : global_state{&gstate},
        task{fn}
  { }

  void run()
  {
    while (!global_state->terminate)
    {
      task(*global_state, this->mbox());
    }
    this->stop();
  }

  agent_ref_type ref()
  {
    return agent_ref_type(this);
  }

};


} // namespace samlib
