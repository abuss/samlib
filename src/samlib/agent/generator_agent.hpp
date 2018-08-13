
#include <unistd.h>
#include <iostream>
#include <chrono>

#include <samlib/agent.hpp>

namespace samlib
{

using namespace std::literals::chrono_literals;

template <typename GS, typename Tin, typename Tout = Tin>
class generator_agent
  : public agent<GS, Tin, Tout>
{
  typedef agent<GS, Tin, Tout>  base_t;
  typedef std::function<Tout(Tin)>      task_t;

  task_t task;

protected:

  using base_t::global_state;
  using base_t::outputs;

public:
  using typename base_t::agent;

  generator_agent(GS &gstate, task_t&& fn)
    : base_t(gstate),
      task(fn)
  { }

  void run()
  {
    size_t ngen = 0;
    while (!global_state->terminate)
    {
      if (auto dat = this->try_receive())
      {
        Tin n = *dat;
        while ((n > 0) && (!global_state->terminate))
        {
          samlib::send_to<0>(outputs,task(n));
          global_state->ngen += 1;
          ++ngen;
          --n;
        }
      }
      else
      {
        // TODO: check if there is a better way to do this
        std::this_thread::sleep_for(20ms);
      }
    }
  }
};

} // namespace samlib
