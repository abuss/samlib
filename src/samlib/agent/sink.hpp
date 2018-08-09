#pragma once

#include <functional>

#include <samlib/agent/base.hpp>

namespace samlib
{

  // template<typename GS, typename T, typename Task>
  template<typename GS, typename T>
  struct sink_agent
    : public samlib::agent_base<T>
  {
    typedef std::function<void(T)> Task;
    GS*   global_state;
    Task  task;

    sink_agent()
      : global_state{nullptr}
    { }
    
    sink_agent(GS& gstate, Task fn)
      : global_state(&gstate),
        task(fn)
    { }
    
    void run()
    {
      while (!global_state->terminate) {
        if (auto data = this->try_receive())
          task(*data);
        else
          std::this_thread::yield();
      }
    }
    
  };

} // namespace samlib
