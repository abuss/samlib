
#include <unistd.h>
#include <iostream>
#include <chrono>

#include <samlib/agent/agent.hpp>

namespace samlib
{

using namespace std::literals::chrono_literals;

template <typename GS, typename Tin, typename Tout = Tin>
struct gen_agent
    : public samlib::agent<GS, Tin, Tout>
{
  typedef samlib::agent<GS, Tin, Tout> base_t;
  typedef std::function<Tout(Tin)> Task;

  using base_t::global_state;
  using base_t::outputs;
  using typename base_t::agent;

  Task task;

  gen_agent(GS &gstate, Task fn)
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
          std::get<0>(outputs)->send(task(n));
          global_state->ngen += 1;
          ++ngen;
          --n;
        }
      }
      else
      {
        std::this_thread::sleep_for(20ms);
      }
    }
  }
};

} // namespace samlib
