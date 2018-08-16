#pragma once

#include <samlib/base.hpp>
#include <samlib/ports.hpp>
#include <samlib/mailbox.hpp>

namespace samlib
{

template <typename State, typename Tin, typename... Tout>
class agent
    : public base<Tin>
{
protected:
  using state_t = State;
  using ports_t = ports<mailbox<Tout> *...>;
  using task_t = std::function<void(state_t&, mailbox<Tin> &, ports_t &)>;

  state_t *global_state;
  ports_t outputs;
  task_t task;

public:
  agent(agent&& other) = default;

  agent(state_t &gstate)
      : global_state{&gstate}
  { }

  agent(state_t &gstate, task_t &&fn)
      : global_state{&gstate},
        task{fn}
  { }

  template <typename... Ps>
  agent &set_outputs(Ps &... ps)
  {
    outputs = make_ports(&ps.mbox()...);
    return *this;
  }

  virtual void run()
  {
    while (!global_state->terminate)
    {
      task(*global_state, this->mbox(), outputs);
    }
  }
};

} // namespace samlib
