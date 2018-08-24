#pragma once

#include <samlib/agent.hpp>

namespace samlib
{

struct empty_state;


template<typename Env>
struct environment
{

  bool terminate = false;

  template <typename... Args, typename Fn>
  samlib::agent<Env, empty_state, Args...> make_agent(Fn&& fn)
  {
    return agent<Env, empty_state, Args...>(*static_cast<Env*>(this), fn);
  }

  // Make an agent with local state
  template <typename... Args, typename LS, typename Fn>
  samlib::agent<Env, LS, Args...> make_agent(LS ls, Fn&& fn)
  {
    return agent<Env, LS, Args...>(*static_cast<Env*>(this), ls, fn);
  }

};

} // namespace samlib
