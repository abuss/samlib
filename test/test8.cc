
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
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
 
  typedef samlib::environment<samlib::base_state> state_t;
  
  state_t st;

  auto& p1 = st.make_agent<size_t,size_t>(samlib::transform(fn_factory("Hugo")));
  auto& p2 = st.make_agent<size_t,size_t>(samlib::transform(fn_factory("Paco")));
  auto& p3 = st.make_agent<size_t,size_t>(samlib::transform(fn_factory("Luis")));

  p1.set_outputs(p2);
  p2.set_outputs(p3);
  p3.set_outputs(p1);

  st.start_agents();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.wait_agents();
}
