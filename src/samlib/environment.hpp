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

};

} // namespace samlib
