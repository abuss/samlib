#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <vector>
#include <samlib/agent.hpp>
#include <samlib/stateless_agent.hpp>
#include <samlib/agent_ref.hpp>

namespace samlib
{

struct empty_state {};

/*!
    Global enviroment definition where the agents share information
*/
  template<typename State=empty_state>
  class environment
    : public State
  {
    struct agent_def {
      samlib::executor* executor = nullptr;
      u_int num_workers;
      bool destroy;
    };

    std::vector<agent_def> agents;
    bool autostart_agents = false;

  public:

    using state_type = State;

    environment(bool auto_start=true)
      : autostart_agents(auto_start)
    { }

    template <typename In>
    using agent_ref_type = agent_ref<agent<State, In>>;

    template <typename In>
    using stateless_agent_ref_type = agent_ref<stateless_agent<In>>;


    template <typename In, typename Fn>
    agent_ref<agent<State, In>> make_agent(Fn&& fn, u_int nworkers=1)
    {
      using agent_t = agent<State, In>;
      agent_ref<agent<State, In>> ref(new agent_t(*static_cast<State*>(this), fn));
      agents.push_back(agent_def{ref.ref_agent().get_executor(), nworkers, true});
      if (autostart_agents)
        ref.start();
      return ref;
    }


    template <typename In, typename Fn>
    agent_ref<stateless_agent<In>> make_stateless_agent(Fn&& fn, u_int nworkers=1)
    {
      using agent_t = stateless_agent<In>;
      agent_ref<stateless_agent<In>> ref(new agent_t(fn));
      agents.push_back(agent_def{ref.ref_agent().get_executor(), nworkers, true});
      if (autostart_agents)
        ref.start();
      return ref;
    }


    template<typename A, typename... Args>
    agent_ref<A> create_agent(Args... args)
    {
      agent_ref<A> ref(new A(*static_cast<State*>(this), args...));
      agents.push_back(agent_def{ref.ref_agent().get_executor(), 1, true});
      if (autostart_agents)
        ref.start();
      return ref;
    }


    template<typename A, typename... Args>
    agent_ref<A> create_stateless_agent(Args... args)
    {
      agent_ref<A> ref(new A(args...));
      agents.push_back(agent_def{ref.ref_agent().get_executor(), 1, true});
      if (autostart_agents)
        ref.start();
      return ref;
    }

    void start_agents()
    {
      // for(auto& a : agents)
        // a.executor->start();
    }

    void wait_for_agents()
    {
      for(auto& a : agents)
        a.executor->join();
    }

    void stop_agents()
    {
      for(auto& a : agents)
        a.executor->request_stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  };

} // namespace samlib
