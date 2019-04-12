
#include <unistd.h>
#include <iostream>

#include <samlib/base.hpp>

template<typename GS, typename T, typename Task>
struct ping_pong_agent
  : public samlib::base<T>
{
  GS*                global_state;
  samlib::base<T>*   mbox_output;

  Task producer;

  ping_pong_agent()
    : global_state{nullptr}, mbox_output{nullptr}
  { }

  ping_pong_agent(GS& gstate, Task fn)
    : global_state(&gstate),
      producer(fn)
  { }

  ~ping_pong_agent() { }

  void set_output(samlib::base<T>& mb_out)
  {
    mbox_output = &mb_out;
  }

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
  
  ping_pong_agent<state,double,ping> p1(st, ping());
  ping_pong_agent<state,double,pong> p2(st, pong());

  p1.set_output(p2);
  p2.set_output(p1);

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
