#pragma once

#include <samlib/agent.hpp>

namespace samlib
{

struct empty_state;


struct environment
{

  bool terminate = false;

  template <typename... Args, typename Fn>
  agent<environment, empty_state, Args...> make_agent(Fn&& fn)
  {
    return agent<environment, empty_state, Args...>(*this, fn);
  }

  template <typename... Args, typename LS, typename Fn>
  agent<environment, LS, Args...> make_agent_with_local(LS ls, Fn&& fn)
  {
    return agent<environment, LS, Args...>(*this, ls, fn);
  }

};

} // namespace samlib
