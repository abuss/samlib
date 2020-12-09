#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the agents can share data
*/

#include <unordered_map>
#include <any>
#include <sstream>

#include <samlib/postoffice.hpp>
#include <samlib/statefull_agent.hpp>
#include <samlib/stateless_agent.hpp>
#include <samlib/agent_ref.hpp>

namespace samlib
{

namespace utility
{

  // TODO: Needed until std::format is included
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

  std::shared_ptr<postoffice>       msg_sys;
  std::shared_ptr<State>            global_state;
  std::shared_ptr<agent_def_t>      agents;
  std::shared_ptr<agent_ref_name_t> agent_names;

  bool autostart_agents = false;
  int  agent_counter = 0;
  bool in_use = false;

public:
  using state_type = State;

  explicit environment(bool auto_start = true)
    : autostart_agents(auto_start)
  {
    msg_sys = std::make_shared<postoffice>();
    global_state = std::make_shared<State>();
    agents = std::make_shared<agent_def_t>();
    agent_names = std::make_shared<agent_ref_name_t>();
  }

  environment(const State& state, bool auto_start = true)
    : autostart_agents(auto_start)
  {
    msg_sys = std::make_shared<postoffice>();
    global_state = std::make_shared<State>(state);
    agents = std::make_shared<agent_def_t>();
    agent_names = std::make_shared<agent_ref_name_t>();
  }

  template<typename In>
  void register_agent(agent_ref<In> ref, std::string name = "", u_int nworkers = 1)
  {
    if (name.empty())
      name = utility::genName(++agent_counter);
    (*agent_names)[name] = ref;
    agents->push_back(agent_def_item({ref.ref_agent(), nworkers}));
    if (autostart_agents) {
      in_use = true;
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
    std::string  name = utility::genName(++agent_counter);
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
    agent_ref<typename A::message_type> ref(std::make_shared<A>(this->make_channel<typename A::message_type>(), args...));
    std::string name = utility::genName(++agent_counter);
    register_agent(ref, name, 1);
    return ref;
  }

  template<typename A, typename... Args>
  agent_ref<typename A::message_type> create_stateless_agent_named(std::string name, Args... args)
  {
    agent_ref<typename A::message_type> ref(std::make_shared<A>(this->make_channel<typename A::message_type>(), args...));
    register_agent(ref, name, 1);
    return ref;
  }

  template<typename T>
  agent_ref<T> get_agent_ref(std::string name)
  {
    if (agent_names->contains(name))
      return std::any_cast<agent_ref<T>>((*agent_names)[name]);
    return agent_ref<T>(nullptr);
  }

  State& state() const
  {
    return *global_state;
  }

  const State& get_state() const
  {
    return *global_state;
  }

  void wait_for_agents()
  {
    for (auto& a : *agents)
      a.ref_worker.join();
  }

  void activate()
  {
    in_use = true;
  }

  void start_agents()
  {
    in_use = true;
    for (auto& a : *agents) {
      a.ref_worker.start(a.num_workers);
    }
  }

  void stop_agents()
  {
    in_use = false;
    msg_sys->close();
    for (auto& a : *agents) {
      a.ref_worker.stop();
    }
    // Extra time required to let the threads get informed about the stop request
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  bool active() const
  {
    return in_use;
  }

  template<typename T>
  constexpr channel<T>& make_channel()
  {
    return msg_sys->make_channel<T>();
  }

  const postoffice& mailboxes() const
  {
    return *msg_sys;
  }

};

} // namespace samlib
