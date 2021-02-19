#pragma once

#include <type_traits>

#include <samlib/base_agent.hpp>

namespace samlib
{

struct empty_state
{ };

template<typename Env, typename Tin, typename State=empty_state>
class agent
  : public base_agent<Tin>, 
    public State
{
protected:
  using base_t = base_agent<Tin>;

  using task_t = std::function<void(agent&)>;

  task_t task;

public:
  Env&   environment;

  constexpr agent(Env& env, task_t fn)
    : base_t{env.template make_channel<Tin>()},
      task{fn},
      environment{env}
  { }

  virtual ~agent() = default;

  void run(const std::stop_token& st)
  {
    while (!st.stop_requested()) {
        task(*this);
    }
  }

};


} // namespace samlib
