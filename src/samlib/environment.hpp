#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <vector>
#include <unordered_map>
#include <any>
#include <sstream>

#include <samlib/agent.hpp>
#include <samlib/stateless_agent.hpp>
#include <samlib/agent_ref.hpp>

namespace samlib
{

namespace utility
{

  // Needed until std::format is included
  std::string genName(int i)
  {
    std::ostringstream res;
    res << "_" << i;
    return res.str();
  }

} // namespace utility


struct empty_state
{ };

/*!
    Global environment definition where the agents share information
*/
template<typename State = empty_state>
class environment
{

  struct agent_def_item
  {
    std::any          ref;
    samlib::executor* executor = nullptr;
    u_int             num_workers;
  };

  using agent_def_t = std::unordered_map<std::string, agent_def_item>;

  std::shared_ptr<State>       _state;
  std::shared_ptr<agent_def_t> _agents;

  bool _autostart_agents = false;
  int  _agent_counter = 0;
  bool _in_use = false;

public:
  using state_type = State;

  explicit environment(bool auto_start = true)
    : _autostart_agents(auto_start)
  {
    _state = std::make_shared<State>();
    _agents = std::make_shared<agent_def_t>();
  }

  environment(const State& state, bool auto_start = true)
    : _autostart_agents(auto_start)
  {
    _state = std::make_shared<State>(state);
    _agents = std::make_shared<agent_def_t>();
  }


  template<typename In>
  using agent_ref_type = agent_ref<agent<environment, In>>;

  template<typename In>
  using stateless_agent_ref_type = agent_ref<stateless_agent<In>>;


  template<typename In, typename Fn>
  agent_ref<agent<environment, In>> make_agent(Fn fn, std::string name = "", u_int nworkers = 1)
  {
    using agent_t = agent<environment, In>;
    agent_ref<agent_t> ref(std::make_shared<agent_t>(*this, fn));
    if (name.empty())
      name = utility::genName(++_agent_counter);
    _agents->operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), nworkers});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start(nworkers);
    }
    return ref;
  }


  template<typename In, typename Fn>
  agent_ref<stateless_agent<In>> make_stateless_agent(Fn fn, std::string name = "", u_int nworkers = 1)
  {
    using agent_t = stateless_agent<In>;
    agent_ref<agent_t> ref(std::make_shared<agent_t>(fn));
    if (name.empty())
      name = utility::genName(++_agent_counter);
    _agents->operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), nworkers});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start(nworkers);
    }
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<A> create_agent(Args... args)
  {
    agent_ref<A> ref(std::make_shared<A>(*this, args...));
    std::string  name = utility::genName(++_agent_counter);
    _agents->    operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), 1});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start();
    }
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<A> create_agent_named(std::string name, Args... args)
  {
    agent_ref<A> ref(std::make_shared<A>(*this, args...));
    if (name.empty())
      name = utility::genName(++_agent_counter);
    _agents->operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), 1});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start();
    }
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<A> create_stateless_agent(Args... args)
  {
    agent_ref<A> ref(std::make_shared<A>(args...));
    std::string  name = utility::genName(++_agent_counter);
    _agents->    operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), 1});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start();
    }
    return ref;
  }

  template<typename A, typename... Args>
  agent_ref<A> create_stateless_agent_named(std::string name, Args... args)
  {
    agent_ref<A> ref(std::make_shared<A>(args...));
    if (name.empty())
      name = utility::genName(++_agent_counter);
    _agents->operator[](name) = agent_def_item({ref, ref.ref_agent().get_executor(), 1});
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start();
    }
    return ref;
  }

  template<typename A>
  agent_ref<A> get_agent_ref(std::string name)
  {
    if (_agents->contains(name))
      return std::any_cast<agent_ref<A>>(_agents->operator[](name).ref);
    return agent_ref<A>(nullptr);
  }

  State& state() const
  {
    return *_state;
  }

  const State& get_state() const
  {
    return *_state;
  }

  void wait_for_agents()
  {
    for (auto& a : *_agents)
      a.second.executor->join();
  }

  void activate()
  {
    _in_use = true;
  }

  void stop_agents()
  {
    _in_use = false;
    for (auto& a : *_agents) {
      a.second.executor->request_stop();
    }
    // Extra time required to let the threads get informed about the stop request
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  bool active() const
  {
    return _in_use;
  }
};

} // namespace samlib
