
#include <unistd.h>
#include <iostream>
#include <tuple>

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto my_generator = [](auto fn)
{
  // size_t ngen = 0;
  using namespace std::literals::chrono_literals; 

  return [fn](auto& gstate, auto& lstate, auto& in_port, auto& out_ports) {
    lstate.sum = 0;
    if (auto dat = in_port.try_receive()) {
      auto n = *dat;
      while ((n > 0) && (!gstate.terminate)) {
        auto val = fn(n);
        lstate.sum += val;
        samlib::send_to<0>(out_ports, val);
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
 
  typedef samlib::environment<samlib::base_state> state_t;

  struct { size_t sum=0; } lst;

  state_t st;

  auto& p1 = st.make_agent<size_t,size_t>(lst, my_generator(gen));
  auto& p2 = st.make_agent<size_t>(samlib::sink(hole));

  p1.set_outputs(p2);

  st.start_agents();

  p1.send(10);

  sleep(5);

  printf("------------ Time's up ---------------\n");
  
  st.wait_agents();

}
