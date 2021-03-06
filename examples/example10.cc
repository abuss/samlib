
#include <unistd.h>
#include <iostream>
#include <vector>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto fn_factory(int num)
{
  return [=](size_t val) {
    printf("P%d -> %lu\n", num, val++);
    return val;
  };
}


int main(int argc, char* argv[])
{

  using env_t = samlib::environment;
  using agent_ref_t = samlib::agent_ref<size_t>;

  int n = 5;
  if (argc > 1)
    n = atoi(argv[1]);

  env_t env;

  std::vector<agent_ref_t> agents(n);

  for (int i = 0; i < n; ++i) {
    agents[i] = env.make_agent<size_t>(samlib::transform(fn_factory(i), agents[(i + 1) % n]));
  }

  //st.start_agents();

  sleep(1);

  agents[0].send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  env.stop_agents();
}
