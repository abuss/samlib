
#include <unistd.h>
#include <iostream>

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
  using env_t =  samlib::environment<>;

  env_t env;
  env_t::stateless_agent_ref_type<size_t> p1,p2;

  p1 = env.make_stateless_agent<size_t>(samlib::stateless::generator(gen,p2));
  p2 = env.make_stateless_agent<size_t>(samlib::stateless::sink(hole));

  // st.start_agents();

  p1.send(100000);

  sleep(1);
  
  printf("------------ Time's up ---------------\n");
  env.stop_agents();

}
