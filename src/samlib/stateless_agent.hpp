#pragma once

#include <samlib/base_agent.hpp>

namespace samlib
{

template <typename Tin>
class stateless_agent
  : public base_agent<Tin>
{
protected:

  using base_t = base_agent<Tin>;
  using task_t = std::function<void(typename base_t::mailbox_type&)>;

  task_t task;

public:
  using agent_ref_type = agent_ref<stateless_agent>;

  constexpr stateless_agent(task_t fn)
    : task{fn}
  { }

  void run(std::stop_token st)
  {
    while (!st.stop_requested())
    {
      task(this->mbox());
    }
  }

  constexpr agent_ref_type ref() noexcept
  {
    return agent_ref_type(this);
  }

};


} // namespace samlib
