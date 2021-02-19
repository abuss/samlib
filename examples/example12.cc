
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
      while ((n > 0)) {
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


struct engine
{
  using env_t = samlib::environment;

  env_t env;
  samlib::agent_ref<size_t> p1, p2;

  template<typename Gen, typename Sink>
  engine(Gen _generator, Sink _sink)
  {
    p1 = env.make_agent<size_t>(my_generator(_generator, p2));
    p2 = env.make_agent<size_t>(samlib::sink(_sink));
  }

  void run_for(size_t n, u_int t)
  {
    auto p1 = env.get_agent_ref<size_t>("_1");
  
    p1.send(n);

    sleep(t);

    printf("------------ Time's up ---------------\n");

    env.stop_agents();

    printf("------------ Clean up ---------------\n");
  }
};


int main()
{
  auto eng = engine(gen, hole);
  eng.run_for(100, 1);
}
