#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <unordered_map>
#include <any>
#include <sstream>

#include <samlib/statefull_agent.hpp>
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
    agent_worker& ref_worker;
    u_int         num_workers;
  };

  using agent_def_t = std::vector<agent_def_item>;
  using agent_ref_name_t = std::unordered_map<std::string, std::any>;

  std::shared_ptr<State>            _state;
  std::shared_ptr<agent_def_t>      _agents;
  std::shared_ptr<agent_ref_name_t> _agent_names;

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
    _agent_names = std::make_shared<agent_ref_name_t>();
  }

  environment(const State& state, bool auto_start = true)
    : _autostart_agents(auto_start)
  {
    _state = std::make_shared<State>(state);
    _agents = std::make_shared<agent_def_t>();
    _agent_names = std::make_shared<agent_ref_name_t>();
  }

  template<typename In>
  void register_agent(agent_ref<In> ref, std::string name = "", u_int nworkers = 1)
  {
    if (name.empty())
      name = utility::genName(++_agent_counter);
    (*_agent_names)[name] = ref;
    _agents->push_back(agent_def_item({ref.ref_agent(), nworkers}));
    if (_autostart_agents) {
      _in_use = true;
      ref.ref_agent().start(nworkers);
    }
  }


  template<typename In, typename Fn>
  agent_ref<In> make_statefull_agent(Fn fn, std::string name = "", u_int nworkers = 1)
  {
    using agent_t = statefull_agent<environment, In>;
    agent_ref<In> ref(std::make_shared<agent_t>(*this, fn));
    register_agent(ref, name, nworkers);
    return ref;
  }


  template<typename In, typename Fn>
  agent_ref<In> make_stateless_agent(Fn fn, std::string name = "", u_int nworkers = 1)
  {
    using agent_t = stateless_agent<In>;
    agent_ref<In> ref(std::make_shared<agent_t>(fn));
    register_agent(ref, name, nworkers);
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<typename A::message_type> create_statefull_agent(Args... args)
  {
    agent_ref<typename A::message_type> ref(std::make_shared<A>(*this, args...));
    std::string  name = utility::genName(++_agent_counter);
    register_agent(ref, name, 1);
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<typename A::message_type> create_statefull_agent_named(std::string name, Args... args)
  {
    agent_ref<typename A::message_type> ref(std::make_shared<A>(*this, args...));
    register_agent(ref, name, 1);
    return ref;
  }


  template<typename A, typename... Args>
  agent_ref<typename A::message_type> create_stateless_agent(Args... args)
  {
    agent_ref<typename A::message_type> ref(std::make_shared<A>(args...));
    std::string name = utility::genName(++_agent_counter);
    register_agent(ref, name, 1);
    return ref;
  }

  template<typename A, typename... Args>
  agent_ref<typename A::message_type> create_stateless_agent_named(std::string name, Args... args)
  {
    agent_ref<typename A::message_type> ref(std::make_shared<A>(args...));
    register_agent(ref, name, 1);
    return ref;
  }

  template<typename T>
  agent_ref<T> get_agent_ref(std::string name)
  {
    if (_agent_names->contains(name))
      return std::any_cast<agent_ref<T>>((*_agent_names)[name]);
    return agent_ref<T>(nullptr);
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
      a.ref_worker.join();
  }

  void activate()
  {
    _in_use = true;
  }

  void start_agents()
  {
    _in_use = true;
    for (auto& a : *_agents) {
      a.ref_worker.start(a.num_workers);
    }
  }

  void stop_agents()
  {
    _in_use = false;
    for (auto& a : *_agents) {
      a.ref_worker.stop();
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
