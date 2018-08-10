
#include <unistd.h>
#include <iostream>

#include <samlib/agent/base.hpp>

template<typename GS, typename T, typename Task>
struct ping_pong_agent
  : public samlib::agent_base<T>
{
  GS*                      global_state;
  samlib::agent_base<T>*   mbox_output;

  Task producer;

  ping_pong_agent()
    : global_state{nullptr}, mbox_output{nullptr}
  { }

  ping_pong_agent(GS& gstate, samlib::agent_base<T>& mb_out, Task fn)
    : global_state(&gstate),
      mbox_output{&mb_out},
      producer(fn)
  { }
  
  void run()
  {
    while (!global_state->terminate) {
      auto data = this->receive();
      auto new_data = producer(data);
      mbox_output->send(*new_data);
    }
  }
};


struct ping
{
  std::optional<double> operator()(double val)
  {
    printf("Ping -> %f\n",val++);
    return val;
  }
};


struct pong
{
  std::optional<double> operator()(double val)
  {
    printf("%f <- Pong\n",val++);
    return val;
  }
};


int main()
{
  struct state { bool terminate = false; };  
  state st;
  
  ping_pong_agent<state,double,ping> p1;
  ping_pong_agent<state,double,pong> p2;

  p1 = ping_pong_agent<state,double,ping>(st, p2, ping());
  p2 = ping_pong_agent<state,double,pong>(st, p1, pong());

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
