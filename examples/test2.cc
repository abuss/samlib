
#include <unistd.h>
#include <iostream>

#include <samlib/base_agent.hpp>
#include <samlib/agent_ref.hpp>

template<typename State, typename T, typename Task>
struct ping_pong_agent
  : public samlib::base_agent<T>
{
  State*                  state;
  using pp_ref_t = samlib::agent_ref<samlib::base_agent<T>>;

  pp_ref_t output;

  Task producer;

  ping_pong_agent()
    : state{nullptr}
  { }

  ping_pong_agent(State& gstate, Task fn)
    : state(&gstate),
      producer(fn)
  { }

  ~ping_pong_agent() { }

  void set_output(pp_ref_t out)
  {
    output = out;
  }

  void run(std::stop_token stoken)
  {
    while (!stoken.stop_requested()) {
      auto data = this->receive();
      auto new_data = producer(data);
      output.send(*new_data);
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
  struct state { };  
  state st;
  
  ping_pong_agent<state,double,ping> p1(st, ping());
  ping_pong_agent<state,double,pong> p2(st, pong());

  p1.set_output(p2.ref());
  p2.set_output(p1.ref());

  p1.start();
  p2.start();

  sleep(1);

  p2.send(1);

  sleep(1);

  printf("------------ Time's up ---------------\n");

  p1.stop();
  p2.stop();

}
