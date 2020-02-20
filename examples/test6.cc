
#include <unistd.h>
#include <iostream>

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto my_generator = [](auto fn, auto& out)
{
  using namespace std::literals::chrono_literals; 
  size_t sum = 0;

  return [fn,&out,&sum](auto& in_port) {
    sum = 0;
    auto n = in_port.receive(); 
    while ((n > 0)) {
      auto val = fn(n);
      sum += val;
      out.send(val);
      --n;
    }
    printf("Sum: %lu\n",sum);
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
 
  using env_t =  samlib::environment<>;

  struct local_state { size_t sum=0; } lst;

  env_t env;

  env_t::stateless_agent_ref_type<size_t> p1;
  env_t::agent_ref_type<size_t> p2;

  p1 = env.make_stateless_agent<size_t>(my_generator(gen,p2));
  p2 = env.make_agent<size_t>(samlib::sink(hole));

  // st.start_agents();

  p1.send(10000);

  sleep(1);

  printf("------------ Time's up ---------------\n");

  env.stop_agents();
  
  printf("------------ Clean up ---------------\n");

}
