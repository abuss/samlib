#pragma once

#include <samlib/base_agent.hpp>

namespace samlib
{

template<typename Env, typename Tin>
class statefull_agent
  : public base_agent<Tin>
{
protected:
  using base_t = base_agent<Tin>;
  using task_t = std::function<void(Env&, typename base_t::mailbox_type&)>;

  Env&   environment;
  task_t task;

public:

  constexpr explicit statefull_agent(Env& env)
    : base_t{env.template make_channel<Tin>()},
      environment{env}
  { }

  constexpr statefull_agent(Env& env, task_t fn)
    : base_t{env.template make_channel<Tin>()},
      environment{env},
      task{fn}
  { }

  virtual ~statefull_agent() = default;

  void run(const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      task(environment, this->mailbox);
    }
  }

};


} // namespace samlib
