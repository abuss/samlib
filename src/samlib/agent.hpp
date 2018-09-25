#pragma once

#include <samlib/base.hpp>
#include <samlib/ports.hpp>
#include <samlib/mailbox.hpp>

namespace samlib
{

struct empty_state {};


template <typename GState, typename LState, typename Tin, typename... Tout>
class agent
    : public base<Tin>
{
protected:
  using base_t = base<Tin>;
  using global_state_t = GState;
  using local_state_t = LState;
  using ports_t = ports<mailbox<Tout> *...>;
  using task_t = std::function<void(global_state_t&, local_state_t&, mailbox<Tin> &, ports_t &)>;

  global_state_t *global_state;
  local_state_t local_state;
  ports_t outputs;
  task_t task;

public:

  agent(global_state_t &gstate)
      : global_state{&gstate}
  { }

  agent(global_state_t &gstate, task_t &&fn)
      : global_state{&gstate},
        task{fn}
  { }

  agent(global_state_t &gstate, local_state_t lstate)
      : global_state{&gstate},
        local_state{lstate}
  { }

  agent(global_state_t &gstate, local_state_t lstate, task_t &&fn)
      : global_state{&gstate},
        local_state{lstate},
        task{fn}
  { }

  template <typename... Ps>
  agent &set_outputs(Ps &... ps)
  {
    outputs = make_ports(&ps.mbox()...);
    return *this;
  }

  /*virtual*/ void run()
  {
    while (!global_state->terminate)
    {
      task(*global_state, local_state, this->mbox(), outputs);
    }
  }
};

} // namespace samlib
