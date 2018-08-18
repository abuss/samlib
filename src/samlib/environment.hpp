#pragma once

#include <samlib/agent.hpp>

namespace samlib
{

struct environment
{

  bool terminate = false;

  template <typename... Args, typename Fn>
  agent<environment, Args...> make_agent(Fn&& fn)
  {
    return agent<environment, Args...>(*this, fn);
  }

  template <typename... Args, typename LS, typename Fn>
  agent_local<environment, LS, Args...> make_agent_with_local(LS ls, Fn&& fn)
  {
    return agent_local<environment, LS, Args...>(*this, ls, fn);
  }

};

} // namespace samlib
