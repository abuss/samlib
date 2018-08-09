
#include <unistd.h>
#include <iostream>

#include <samlib/agent/agent.hpp>

namespace samlib
{

template <typename GS, typename Tin>
struct periodic_agent
    : public samlib::agent<GS, Tin>
{
  typedef samlib::agent<GS, Tin> base_t;
  typedef std::function<void(GS &)> task_t;

  using base_t::global_state;
  using typename base_t::agent;

  size_t delay;
  task_t task;
  bool   start_delay;
  
  periodic_agent(GS &gstate, size_t d, task_t fn, bool start_with_delay=true)
    : base_t{gstate},
      delay(d),
      task(fn),
      start_delay{start_with_delay}
  { }

  void run()
  {
    if (start_delay)
      while (!global_state->terminate) {
        usleep(delay);
        task(*global_state);
      }
    else
      while (!global_state->terminate) {
        task(*global_state);
        usleep(delay);
      }
  }
};

} // namespace samlib
