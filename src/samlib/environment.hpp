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

    template <typename In, typename LState=empty_state>
    using agent_ref_type = agent_ref<agent<Env, LState, In>>;

    template <typename In, typename Fn>
    agent_ref_type<In> make_agent(Fn&& fn, u_int nworkers=1)
    {
      typedef agent<Env, empty_state, In> agent_t;
      agent_t* ptr =  new agent_t(*static_cast<Env*>(this), fn);
      agents.push_back(agent_def{ptr, nworkers, true});
      return ptr->ref();
    }


    // Make an agent with local state
    template <typename In, typename LS, typename Fn>
    agent_ref_type<In,LS> make_agent(LS ls, Fn&& fn, u_int nworkers)
    {
      typedef agent<Env, LS, In>                agent_t;
      typedef typename agent_t::agent_ref_type  agent_ref_t;
      agent_t* ptr =  new agent_t(*static_cast<Env*>(this), ls, fn);
      agents.push_back(agent_def{ptr, nworkers, true});
      return agent_ref_t(ptr);
    }


    /*
    template<typename A, typename... Args>
    A& create_agent(Args... args)
    {
      A* ptr = new A(*static_cast<Env*>(this), args...);
      agents.push_back(agent_def{ptr, 1, true});
      return *ptr;
    }
  */

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
