
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto my_generator = [](auto fn, auto& out)
{
  // size_t ngen = 0;
  using namespace std::literals::chrono_literals; 
  size_t sum = 0;

  return [fn,&out,&sum](auto& gstate, auto& in_port) {
    sum = 0;
    if (auto dat = in_port.try_receive()) {
      auto n = *dat;
      while ((n > 0) && (!gstate.terminate)) {
        auto val = fn(n);
        sum += val;
        out.send(val);
        --n;
      }
      printf("Sum: %lu\n",sum);
    }
    else {
      std::this_thread::sleep_for(1us);
    }
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
 
  typedef samlib::environment<samlib::base_state> env_t;

  struct local_state { size_t sum=0; } lst;

  env_t st;

  env_t::agent_ref_type<size_t> p1;
  env_t::agent_ref_type<size_t> p2;

  p1 = st.make_agent<size_t>(my_generator(gen,p2),1);
  p2 = st.make_agent<size_t>(samlib::sink(hole));

  st.start_agents();

  p1.send(10);

  sleep(5);

  printf("------------ Time's up ---------------\n");
  
  st.wait_agents();

}
