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


// TODO: review if it is possible define only one agent definition
// Agent with support of global and local state
template <typename GState, typename LState, typename Tin, typename... Tout>
class agent_local
    : public base<Tin>
{
protected:
  using global_state_t = GState;
  using local_state_t = LState;
  using ports_t = ports<mailbox<Tout> *...>;
  using task_t = std::function<void(global_state_t&, local_state_t&, mailbox<Tin> &, ports_t &)>;

  global_state_t *global_state;
  local_state_t local_state;
  ports_t outputs;
  task_t task;

public:
  agent_local(agent_local&& other) = default;

  agent_local(global_state_t &gstate)
      : global_state{&gstate}
  { }

  agent_local(global_state_t &gstate, local_state_t&& lstate)
      : global_state{&gstate},
        local_state{lstate}
  { }

  agent_local(global_state_t &gstate, local_state_t lstate, task_t &&fn)
      : global_state{&gstate},
        local_state{lstate},
        task{fn}
  { }

  template <typename... Ps>
  agent_local &set_outputs(Ps &... ps)
  {
    outputs = make_ports(&ps.mbox()...);
    return *this;
  }

  virtual void run()
  {
    while (!global_state->terminate)
    {
      task(*global_state, local_state, this->mbox(), outputs);
    }
  }
};


// Agent with support of global and local state
template <typename GState, typename LState, typename Tin, typename... Tout>
class agent_l
    : public agent<GState,Tin,Tout...>
{
protected:
  using base = agent<GState,Tin,Tout...>
  using global_state_t = base::global_state_t;
  using local_state_t = LState;
  // using ports_t = ports<mailbox<Tout> *...>;
  using task_t = std::function<void(global_state_t&, local_state_t&, mailbox<Tin> &, ports_t &)>;

  // global_state_t *global_state;
  local_state_t local_state;
  // ports_t outputs;
  task_t task;

public:
  agent_l(agent_l&& other) = default;

  agent_l(global_state_t &gstate)
      : global_state{&gstate}
  { }

  agent_l(global_state_t &gstate, local_state_t&& lstate)
      : global_state{&gstate},
        local_state{lstate}
  { }

  agent_l(global_state_t &gstate, local_state_t lstate, task_t &&fn)
      : global_state{&gstate},
        local_state{lstate},
        task{fn}
  { }

};

} // namespace samlib
