#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <vector>
#include <samlib/agent.hpp>
#include <samlib/agent_ref.hpp>

namespace samlib
{

  struct base_state {
    bool terminate = false;
  };


/*!
    Global enviroment definition where the agents share information
*/
  template<typename Env>
  class environment
    : public Env
  {
    struct agent_def {
      samlib::executor* executor;
      u_int num_workers;
      bool destroy;
    };

    std::vector<agent_def> agents;
    bool autostart_agents = false;

  public:

    environment(bool auto_start=false)
      : autostart_agents(auto_start)
    { }

    ~environment()
    {
      stop_agents();
      // wait_agents();
      for(auto& a : agents) {
        if (a.executor!=nullptr && a.destroy) {
          delete a.executor;
          a.executor = nullptr;
        }
      }
    }

    template <typename In>
    using agent_ref_type = agent_ref<agent<Env, In>>;


    template <typename In, typename Fn>
    agent_ref_type<In> make_agent(Fn&& fn, u_int nworkers=1)
    {
      typedef agent<Env, In> agent_t;
      agent_t* ptr = new agent_t(*static_cast<Env*>(this), fn);
      agents.push_back(agent_def{ptr, nworkers, true});
      if (autostart_agents)
        ptr->start(nworkers);
      return ptr->ref();
    }


    template<typename A, typename... Args>
    typename A::agent_ref_type create_agent(Args... args)
    {
      typedef typename A::agent_ref_type agent_ref_t;
      A* ptr = new A(*static_cast<Env*>(this), args...);
      agents.push_back(agent_def{ptr, 1, true});
      if (autostart_agents)
        ptr->start(1);
      return agent_ref_t(ptr);
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
      for(auto& a : agents)
        a.executor->stop();
    }
  };

} // namespace samlib
