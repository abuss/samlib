
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
};

size_t pong(size_t val)
{
  printf("%lu <- Pong\n", val++);
  return val;
};


int main()
{
 
  typedef samlib::environment<samlib::base_state> state_t;
  
  state_t st;

  // auto p1 = agent_t(st, samlib::transform(ping));
  // auto p2 = agent_t(st, samlib::transform(pong));

  auto p1 = st.make_agent<size_t,size_t>(samlib::transform(ping));
  auto p2 = st.make_agent<size_t,size_t>(samlib::transform(pong));


  p1.set_outputs(p2);
  p2.set_outputs(p1);

  p1.start();
  p2.start();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");
  st.terminate = true;
  p1.wait();
  p2.wait();
}
