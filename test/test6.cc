
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

  return [fn,&out](auto& gstate, auto& lstate, auto& in_port) {
    lstate.sum = 0;
    if (auto dat = in_port.try_receive()) {
      auto n = *dat;
      while ((n > 0) && (!gstate.terminate)) {
        auto val = fn(n);
        lstate.sum += val;
        out.send(val);
        --n;
      }
      printf("Sum: %lu\n",lstate.sum);
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

  env_t::agent_ref_type<size_t, local_state> p1;
  env_t::agent_ref_type<size_t> p2;

  p1 = st.make_agent<size_t>(lst, my_generator(gen,p2),1);
  p2 = st.make_agent<size_t>(samlib::sink(hole));

  st.start_agents();

  p1.send(10);

  sleep(5);

  printf("------------ Time's up ---------------\n");
  
  st.wait_agents();

}
