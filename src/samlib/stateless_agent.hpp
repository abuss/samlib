#pragma once

#include <samlib/base_agent.hpp>

namespace samlib
{

template<typename Tin>
class stateless_agent
  : public base_agent<Tin>
{
protected:
  using base_t = base_agent<Tin>;
  using task_t = std::function<void(typename base_t::mailbox_type&)>;

  task_t task;

public:
  constexpr explicit stateless_agent(task_t fn)
    : task{fn}
  { }

  virtual ~stateless_agent() = default;

  void run(const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      task(this->mbox());
    }
  }

};


} // namespace samlib
