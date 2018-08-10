#pragma once

#include <functional>

#include <samlib/agent/base.hpp>

// TODO: reimplement using agent instead of base
namespace samlib
{

  template<typename GS, typename T>
  struct sink
    : public base<T>
  {
    // typedef std::function<void(T)> task_t;
    using task_t = std::function<void(T)>;
    GS*     global_state;
    task_t  task;

    sink()
      : global_state{nullptr}
    { }
    
    sink(GS& gstate, task_t&& fn)
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
