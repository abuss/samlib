
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


size_t ping(size_t val)
{
  // printf("Ping -> %lu\n", val++);
  return val+1;
}

size_t pong(size_t val)
{
  // printf("%lu <- Pong\n", val++);
  return val+1;
}


int main()
{
  using env_t = samlib::environment<>; 
  
  env_t st;
  size_t nmsg =0;

  env_t::agent_ref_type<size_t> p1,p2;

  p1 = st.make_agent<size_t>(samlib::transform([&](auto val){++nmsg; return ping(val); }, p2));
  p2 = st.make_agent<size_t>(samlib::transform([&](auto val){++nmsg; return pong(val); }, p1));

  st.start_agents();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");
  std::cout << "last:" << nmsg << std::endl;
  
  st.stop_agents();
}
