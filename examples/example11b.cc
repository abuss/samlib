
#include <unistd.h>
#include <iostream>
#include <random>
#include <tuple>
#include <chrono>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


using vect_t = std::vector<int>;
using out_t = std::pair<std::string, int>;
std::default_random_engine rgen;

vect_t generate(size_t)
{
  vect_t                             data(10);
  std::uniform_int_distribution<int> distribution(1, 100);
  for (auto& v : data) {
    v = distribution(rgen);
    std::cout << v << ",";
  }
  std::cout << std::endl;
  return data;
}

template<typename Fn, typename O>
auto transform_state(Fn fn, O& out)
{
  return [fn, &out](auto& agent) {
    if (auto data = agent.try_receive()) {
      out.send(fn(agent, std::move(*data)));
    }
  };
}

struct my_state {
  int max = 0;
  int min = 0;
};

out_t min_value(my_state& state, const vect_t& data)
{
  state.min++;
  if (!data.empty())
    return out_t("Min", *std::min_element(data.begin(), data.end()));
  return out_t("Done", 0);
}

out_t max_value(my_state& state, const vect_t& data)
{
  state.max++;
  if (!data.empty())
    return out_t("Max", *std::max_element(data.begin(), data.end()));
  return out_t("Done", 0);
}

template<typename T>
void output(T val)
{
  printf("%s: %d\n", val.first.c_str(), val.second);
}


int main()
{
  using env_t = samlib::environment;
  using agent_min_max_t = env_t::stateful_agent<vect_t, my_state>;

  env_t env;
  samlib::agent_ref<size_t> p_gen;
  samlib::agent_ref<vect_t> p_split, p_min, p_max;
  samlib::agent_ref<out_t>  p_out;

  printf("------------ First version ---------------\n");

  p_gen = env.make_agent<size_t>(samlib::generator(generate, p_split));
  p_split = env.make_agent<vect_t>(samlib::splitter(p_min, p_max));
  p_max = env.make_agent<vect_t, my_state>(transform_state(max_value, p_out));
  p_min = env.make_agent<vect_t, my_state>(transform_state(min_value, p_out));
  p_out = env.make_agent<out_t>(samlib::sink(output<out_t>));

  //st.start_agents();

  sleep(1);

  p_gen.send(5);

  sleep(1);

  std::cout << "Max value: " << samlib::state_of<agent_min_max_t>(p_max).max << std::endl;
  std::cout << "Min value: " << samlib::state_of<agent_min_max_t>(p_min).min << std::endl;
  printf("------------ Second version ---------------\n");

  auto p_gen2 = env.make_agent<size_t>(
    samlib::generator(generate,
      env.make_agent<vect_t>(
        samlib::splitter(
          env.make_agent<vect_t, my_state>(transform_state(min_value, p_out)),
          env.make_agent<vect_t, my_state>(transform_state(max_value, p_out))
        )
      )
    )
  );

  //st.start_agents();

  sleep(1);

  p_gen2.send(2);

  sleep(1);

  printf("------------ Time's up ---------------\n");

  env.stop_agents();
}