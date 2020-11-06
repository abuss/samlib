
#include <unistd.h>
#include <iostream>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


size_t ping(size_t val)
{
  return val + 1;
}

size_t pong(size_t val)
{
  return val + 1;
}


int main()
{
  using env_t = samlib::environment<>;

  env_t  env;
  size_t nmsg = 0;

  samlib::agent_ref<size_t> p1, p2;

  p1 = env.make_statefull_agent<size_t>(samlib::transform([&](auto val) {++nmsg; return ping(val); }, p2));
  p2 = env.make_statefull_agent<size_t>(samlib::transform([&](auto val) {++nmsg; return pong(val); }, p1));

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");
  std::cout << "last:" << nmsg << std::endl;

  env.stop_agents();
}
