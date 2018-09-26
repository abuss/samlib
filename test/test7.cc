
#include <unistd.h>
#include <iostream>
#include <tuple>
#include <vector>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


struct payload
{
  std::vector<size_t> data;
  size_t n=0;

  payload() : data{1000}, n(0) {}
};

payload ping(payload val)
{
  val.n++;
  printf("Ping -> %lu (%lu)\n", ++val.data[0],val.n);
  return val;
};

payload pong(payload val)
{
  val.n++;
  printf("%lu (%lu) <- Pong\n", ++val.data[1], val.n);
  return val;
};


int main()
{
 
  typedef samlib::environment<samlib::base_state> state_t;
  
  state_t st;

  auto& p1 = st.make_agent<payload,payload>(samlib::transform(ping));
  auto& p2 = st.make_agent<payload,payload>(samlib::transform(pong));

  p1.set_outputs(p2);
  p2.set_outputs(p1);

  st.start_agents();

  payload vec; //{1000};

  sleep(1);

  p2.send(vec);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.wait_agents();
}
