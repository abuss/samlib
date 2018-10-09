
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
 
  typedef samlib::environment<samlib::base_state> state_t;

  state_t st;

  auto& p1 = st.make_agent<size_t,size_t>(samlib::generator(gen));
  auto& p2 = st.make_agent<size_t>(samlib::sink(hole));

  p1.set_outputs(p2);

  st.start_agents();

  p1.send(10000);

  sleep(3);
  
  printf("------------ Time's up ---------------\n");
  st.wait_agents();

}
