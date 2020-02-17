
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/agent_ref.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto fn_factory(std::string name)
{
  return [=](size_t val) {
    printf("%s -> %lu\n",name.c_str(),val++);
    return val;
  };
}


int main()
{
 
  using env_t =  samlib::environment<>;
  
  env_t st;

  env_t::agent_ref_type<size_t> p1,p2,p3;

  p1 = st.make_agent<size_t>(samlib::transform(fn_factory("Hugo"),p2));
  p2 = st.make_agent<size_t>(samlib::transform(fn_factory("Paco"),p3));
  p3 = st.make_agent<size_t>(samlib::transform(fn_factory("Luis"),p1));

  st.start_agents();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.stop_agents();
}
