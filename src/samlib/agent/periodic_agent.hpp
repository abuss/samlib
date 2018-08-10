
#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>

namespace samlib
{

template <typename GS, typename Tin>
class periodic_agent
  : public agent<GS, Tin>
{
  typedef agent<GS, Tin>             base_t;
  typedef std::function<void(GS &)>  task_t;

  size_t delay;
  task_t task;
  bool   start_delay;

protected:

  using base_t::global_state;
  
public:

  using typename base_t::agent;

  periodic_agent(GS &gstate, size_t d, task_t fn, bool start_with_delay=true)
    : base_t{gstate},
      delay(d),
      task(fn),
      start_delay{start_with_delay}
  { }

  void run()
  {
    if (start_delay)
        usleep(delay);  
    while (!global_state->terminate) {
        task(*global_state);
        usleep(delay);
    }
  }
};

} // namespace samlib::agent
