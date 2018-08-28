#pragma once

#include <vector>
#include <samlib/agent.hpp>

namespace samlib
{

struct empty_state;

struct base_state {
  bool terminate = false;
};

template<typename Env>
class environment
  : public base_state
{

  std::vector<samlib::executor*> agents;

public:

  ~environment()
  {
    wait_agents();
    for(auto& a : agents) {
      if (a)
        delete a;
        a = nullptr;
    }
  }

  template <typename... Args, typename Fn>
  samlib::agent<Env, empty_state, Args...>& make_agent(Fn&& fn)
  {
    typedef agent<Env, empty_state, Args...> agent_t;
    agent_t* ptr =  new agent_t(*static_cast<Env*>(this), fn);
    agents.push_back(ptr);
    return *ptr;
  }

  // Make an agent with local state
  template <typename... Args, typename LS, typename Fn>
  samlib::agent<Env, LS, Args...>& make_agent(LS ls, Fn&& fn)
  {
    typedef agent<Env, LS, Args...> agent_t;
    agent_t* ptr = new agent_t(*static_cast<Env*>(this), ls, fn);
    agents.push_back(ptr);
    return *ptr;
  }

  void start_agents()
  {
    this->terminate = false;
    for(auto& a : agents)
      a->start();
  }

  void wait_agents()
  {
    stop_agents();
    for(auto& a : agents)
      a->wait();
  }

  void stop_agents()
  {
    this->terminate = true;
  }
};

} // namespace samlib
