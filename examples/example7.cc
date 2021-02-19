
#include <unistd.h>
#include <iostream>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto my_generator = [](auto fn, auto& out) {
  using namespace std::literals::chrono_literals;

  return [fn, &out](auto& agent) {
    size_t sum = 0;
    if (auto data = agent.receive()) {
      auto n = *data;
      while (n > 0 && agent.environment.active()) {
        auto val = fn(n);
        sum += val;
        out.send(val);
        --n;
      }
    }
    printf("Sum: %lu\n", sum);
  };
};

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

  using env_t = samlib::environment;

  env_t env;

  samlib::agent_ref<size_t> p1, p2;

  p1 = env.make_agent<size_t>(my_generator(gen, p2));
  p2 = env.make_agent<size_t>(samlib::sink(hole));

  // st.start_agents();

  p1.send(1000000);

  sleep(1);

  printf("------------ Time's up ---------------\n");

  env.stop_agents();

  printf("------------ Clean up ---------------\n");
}
