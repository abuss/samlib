
#include <unistd.h>
#include <iostream>
#include <tuple>

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
  typedef samlib::environment<samlib::base_state> env_t;

  env_t st;

  env_t::agent_ref_type<size_t> p1,p2;

  p1 = st.make_agent<size_t>(samlib::transform(ping,p2));
  p2 = st.make_agent<size_t>(samlib::transform(pong,p1));

  st.start_agents();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.wait_agents();
}
