
#include <unistd.h>
#include <iostream>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto fn_factory(std::string name)
{
  return [=](size_t val) {
    printf("%s -> %lu\n", name.c_str(), val++);
    return val;
  };
}


int main()
{

  using env_t = samlib::environment<>;

  env_t env;

  env_t::stateless_agent_ref_type<size_t> p1, p2, p3;

  p1 = env.make_stateless_agent<size_t>(samlib::stateless::transform(fn_factory("Hugo"), p2));
  p2 = env.make_stateless_agent<size_t>(samlib::stateless::transform(fn_factory("Paco"), p3));
  p3 = env.make_stateless_agent<size_t>(samlib::stateless::transform(fn_factory("Luis"), p1));

  // st.start_agents();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  env.stop_agents();
}
