#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


size_t ping(size_t val)
{
  printf("Ping -> %lu\n", val++);
  return val;
}

size_t pong(size_t val)
{
  printf("%lu <- Pong\n", val++);
  return val;
}

int main()
{
  using env_t = samlib::environment<>;

  env_t env;

  using agent_t = samlib::agent<env_t, size_t>;
  samlib::agent_ref<agent_t> p1, p2;

  p1 = env.create_agent<agent_t>(transform(ping, p2));
  p2 = env.create_agent<agent_t>(transform(pong, p1));

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  env.stop_agents();
}
