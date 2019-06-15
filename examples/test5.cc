
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


void hole(size_t val)
{
  printf("To the hole -> %lu\n", val);
}

size_t gen(size_t val)
{
  printf("Generate -> %lu\n", ++val);
  return val;
}


int main()
{
  typedef samlib::environment<samlib::base_state> env_t;

  env_t st;
  env_t::agent_ref_type<size_t> p1,p2;

  p1 = st.make_agent<size_t>(samlib::generator(gen,p2));
  p2 = st.make_agent<size_t>(samlib::sink(hole));

  st.start_agents();

  p1.send(10000);

  sleep(1);
  
  printf("------------ Time's up ---------------\n");
  st.wait_agents();

}
