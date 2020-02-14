#pragma once

#include <samlib/base.hpp>
#include <samlib/mailbox.hpp>

namespace samlib
{

struct empty_state {};

template <typename A>
class agent_ref;

template <typename State, typename Tin>
class agent
  : public base<Tin>
{
protected:
  using base_t = base<Tin>;
  using task_t = std::function<void(State&, mailbox<Tin> &)>;

  State* state;
  task_t task;

public:
  using agent_ref_type = agent_ref<agent>;

  constexpr agent(State& gstate)
    : state{&gstate}
  { }

  constexpr agent(State& gstate, task_t&& fn)
    : state{&gstate},
      task{fn}
  { }

  void run(std::stop_token st)
  {
    while (!st.stop_requested())
    {
      task(*state, this->mbox());
    }
    this->stop();
  }

  agent_ref_type ref() noexcept
  {
    return agent_ref_type(this);
  }

};


} // namespace samlib
