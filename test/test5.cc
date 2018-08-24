
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


void hole(size_t val)
{
  printf("To the hole -> %lu\n", val);
};

size_t gen(size_t val)
{
  printf("Generate -> %lu\n", ++val);
  return val;
};


int main()
{
 
  typedef samlib::environment<samlib::base_state> state_t;
  // typedef samlib::agent<state_t,size_t> agent_sink_t;

  state_t st;

  auto p1 = st.make_agent<size_t,size_t>(samlib::generator(gen));
  auto p2 = st.make_agent<size_t>(samlib::sink(hole));
  // auto p2 = agent_sink_t(st, samlib::sink(hole));


  p1.set_outputs(p2);
  // p2.set_outputs(p1);

  p1.start();
  p2.start();

  // sleep(1;

  p1.send(10000);

  sleep(10);
  printf("------------ Time's up ---------------\n");
  st.terminate = true;
  p1.wait();
  p2.wait();
}
