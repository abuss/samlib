
#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>


template<typename GS, typename Tin, typename Tout=Tin>
struct ping_pong_agent
  : public samlib::agent<GS,Tin,Tout>
{
  typedef samlib::agent<GS,Tin,Tout>     base_t;
  typedef std::function<Tout(Tin)>       task_t;

  using base_t::global_state;
  using base_t::outputs;

  task_t task;

  ping_pong_agent(GS &gstate, task_t fn)
    : base_t{gstate},
      task{fn}
  { }
  
  void run()
  {
    while (!global_state->terminate) {
      Tin data = this->receive();
      Tout new_data = task(data);
      outputs.template send_to<0>(new_data);
    }
  }
};


double ping(double val)
{
    printf("Ping -> %f\n",val++);
    return val;
};


double pong(double val)
{
    printf("%f <- Pong\n",val++);
    return val;
};


int main()
{
  struct state { bool terminate = false; };  
  state st;
  
  typedef ping_pong_agent<state,double> agent_t;

  auto p1 = agent_t(st, ping);
  auto p2 = agent_t(st, pong);

  p1.set_outputs(p2);
  p2.set_outputs(p1);

  p1.start();
  p2.start();

  sleep(1);

  p2.send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");
  st.terminate = true;
  p1.wait();
  p2.wait();


}
