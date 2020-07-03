
#include <unistd.h>
#include <iostream>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto my_generator = [](auto fn, auto& out) {
  using namespace std::literals::chrono_literals;

  return [fn, &out](auto& in_port) {
    size_t sum = 0;
    if (auto data = in_port.receive()) {
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
  using env_t = samlib::environment<>;

  env_t env;

  template<typename Gen, typename Sink>
  engine(Gen _generator, Sink _sink)
  {
    env_t::stateless_agent_ref_type<size_t> p1;
    env_t::agent_ref_type<size_t>           p2;

    p1 = env.make_stateless_agent<size_t>(my_generator(_generator, p2));
    p2 = env.make_agent<size_t>(samlib::sink(_sink));
  }

  void run_for(int n, int t)
  {
    auto p1 = env.template get_agent_ref<samlib::stateless_agent<size_t>>("_1");
    // auto p2 = env.template get_agent_ref<samlib::agent<env_t, size_t>>("_2");
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
