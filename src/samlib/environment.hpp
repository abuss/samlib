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

  std::vector<std::pair<samlib::executor*,bool>> agents;

public:

  ~environment()
  {
    wait_agents();
    for(auto& a : agents) {
      if (a.first!=nullptr && a.second) {
        delete a.first;
        a.first = nullptr;
      }
    }
  }

  template <typename... Args, typename Fn>
  samlib::agent<Env, empty_state, Args...>& make_agent(Fn&& fn)
  {
    typedef agent<Env, empty_state, Args...> agent_t;
    agent_t* ptr =  new agent_t(*static_cast<Env*>(this), fn);
    agents.push_back(std::make_pair(ptr,true));
    return *ptr;
  }

  // Make an agent with local state
  template <typename... Args, typename LS, typename Fn>
  samlib::agent<Env, LS, Args...>& make_agent(LS ls, Fn&& fn)
  {
    typedef agent<Env, LS, Args...> agent_t;
    agent_t* ptr = new agent_t(*static_cast<Env*>(this), ls, fn);
    agents.push_back(std::make_pair(ptr,true));
    return *ptr;
  }

  template<typename A, typename... Args>
  A& create_agent(Args... args)
  {
    A* ptr = new A(*static_cast<Env*>(this), args...);
    agents.push_back(std::make_pair(ptr,true));
    return *ptr;
  }

  void start_agents()
  {
    this->terminate = false;
    for(auto& a : agents)
      a.first->start();
  }

  void wait_agents()
  {
    stop_agents();
    for(auto& a : agents)
      a.first->wait();
  }

  void stop_agents()
  {
    this->terminate = true;
  }
};

} // namespace samlib
