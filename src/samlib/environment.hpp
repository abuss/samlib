#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <vector>
#include <samlib/agent.hpp>

namespace samlib
{

struct empty_state;

struct base_state {
  bool terminate = false;
};

/*!
    Global enviroment definition where the agents share information
*/
template<typename Env>
class environment
  : public base_state
{
  struct agent_def {
    samlib::executor* executor;
    u_int num_workers;
    bool destroy;
  };

  std::vector<agent_def> agents;

public:

  ~environment()
  {
    wait_agents();
    for(auto& a : agents) {
      if (a.executor!=nullptr && a.destroy) {
        delete a.executor;
        a.executor = nullptr;
      }
    }
  }

  template <typename... Args, typename Fn>
  samlib::agent<Env, empty_state, Args...>& make_agent(Fn&& fn)
  {
    typedef agent<Env, empty_state, Args...> agent_t;
    agent_t* ptr =  new agent_t(*static_cast<Env*>(this), fn);
    agents.push_back(agent_def{ptr, 1, true});
    return *ptr;
  }

  template <typename... Args, typename Fn>
  samlib::agent<Env, empty_state, Args...>& make_agent_n(Fn&& fn, u_int nworkers)
  {
    typedef agent<Env, empty_state, Args...> agent_t;
    agent_t* ptr =  new agent_t(*static_cast<Env*>(this), fn);
    agents.push_back(agent_def{ptr, nworkers, true});
    return *ptr;
  }

  // Make an agent with local state
  template <typename... Args, typename LS, typename Fn>
  samlib::agent<Env, LS, Args...>& make_agent(LS ls, Fn&& fn)
  {
    typedef agent<Env, LS, Args...> agent_t;
    agent_t* ptr = new agent_t(*static_cast<Env*>(this), ls, fn);
    agents.push_back(agent_def{ptr, 1, true});
    return *ptr;
  }

  template <typename... Args, typename LS, typename Fn>
  samlib::agent<Env, LS, Args...>& make_agent_n(LS ls, Fn&& fn, u_int nworkers)
  {
    typedef agent<Env, LS, Args...> agent_t;
    agent_t* ptr = new agent_t(*static_cast<Env*>(this), ls, fn);
    agents.push_back(agent_def{ptr, nworkers, true});
    return *ptr;
  }

  template<typename A, typename... Args>
  A& create_agent(Args... args)
  {
    A* ptr = new A(*static_cast<Env*>(this), args...);
    agents.push_back(agent_def{ptr, 1, true});
    return *ptr;
  }

  void start_agents()
  {
    this->terminate = false;
    for(auto& a : agents)
      a.executor->start(a.num_workers);
  }

  void wait_agents()
  {
    stop_agents();
    for(auto& a : agents)
      a.executor->wait();
  }

  void stop_agents()
  {
    this->terminate = true;
  }
};

} // namespace samlib
