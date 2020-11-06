#pragma once

#include <samlib/base_agent.hpp>

namespace samlib
{

template<typename State, typename Tin>
class statefull_agent
  : public base_agent<Tin>
{
protected:
  using base_t = base_agent<Tin>;
  using task_t = std::function<void(State&, typename base_t::mailbox_type&)>;

  State* state;
  task_t task;

public:

  constexpr explicit statefull_agent(State& gstate)
    : state{&gstate}
  { }

  constexpr statefull_agent(State& gstate, task_t fn)
    : state{&gstate},
      task{fn}
  { }

  virtual ~statefull_agent() = default;

  void run(const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      task(*state, this->mbox());
    }
  }

};


} // namespace samlib
