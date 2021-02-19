#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>
#include <samlib/environment.hpp>


template<typename Fn, typename O>
auto transform(Fn fn, O& out)
{
  return [fn, &out](auto& agent) {
    if (auto data = agent.try_receive()) {
      out.send(fn(agent, std::move(*data)));
    }
  };
}


struct state {
    int value;
};

size_t ping(state& st, size_t val)
{
  // printf("Ping -> %lu\n", val++);
  val++;
  st.value += 1;
  return val;
}

size_t pong(state& st, size_t val)
{
  // printf("%lu <- Pong\n", val++);
  val++;
  st.value += 1;
  return val;
}


int main()
{
  using env_t = samlib::environment;

  env_t env;

  using agent_t = env_t::stateful_agent<size_t, state>;
  samlib::agent_ref<size_t> p1, p2;

  p1 = env.create_agent<agent_t>(transform(ping, p2));
  p2 = env.create_agent<agent_t>(transform(pong, p1));
  
  samlib::state_of<agent_t>(p1).value = 0;
  samlib::state_of<agent_t>(p2).value = 0;

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  env.stop_agents();
  std::cout << "P1 value: " << samlib::state_of<agent_t>(p1).value << std::endl;
  std::cout << "P2 value: " << samlib::state_of<agent_t>(p2).value << std::endl;

}
