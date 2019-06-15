
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

  payload(size_t sz) : data(sz), n(0) {}

  payload() : n(0) {}

  bool is_valid() const { return !data.empty(); }
};

payload ping(payload val)
{
  val.n++;
  if (val.is_valid())
    printf("Ping -> %lu (%lu)\n", ++val.data[0],val.n);
  return val;
}

payload pong(payload val)
{
  val.n++;
  if (val.is_valid())
    printf("%lu (%lu) <- Pong\n", ++val.data[1], val.n);
  return val;
}


int main()
{
 
  typedef samlib::environment<samlib::base_state> env_t;
  
  env_t st;

  env_t::agent_ref_type<payload> p1,p2;

  p1 = st.make_agent<payload>(samlib::transform(ping,p2));
  p2 = st.make_agent<payload>(samlib::transform(pong,p1));

  st.start_agents();

  payload vec(1000); //{1000};

  sleep(1);

  p2.send(vec);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.wait_agents();
}
