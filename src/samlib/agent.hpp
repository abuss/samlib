#pragma once

#include <samlib/base_agent.hpp>

namespace samlib
{

template <typename State, typename Tin>
class agent
  : public base_agent<Tin>
{
protected:

  using base_t = base_agent<Tin>;
  using task_t = std::function<void(State&, typename base_t::mailbox_type&)>;

  State* state;
  task_t task;

public:
  using agent_ref_type = agent_ref<agent>;

  constexpr agent(State& gstate)
    : state{&gstate}
  { }

  constexpr agent(State& gstate, task_t fn)
    : state{&gstate},
      task{fn}
  { }

  void run(std::stop_token st)
  {
    while (!st.stop_requested())
    {
      task(*state, this->mbox());
    }
  }

  constexpr agent_ref_type ref() noexcept
  {
    return agent_ref_type(this);
  }

};


} // namespace samlib
