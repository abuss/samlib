
#include <unistd.h>
#include <iostream>

#include <samlib/base_agent.hpp>
#include <samlib/agent_ref.hpp>

template<typename State, typename T, typename Task>
struct ping_pong_agent
  : public samlib::base_agent<T>
{
  State*                  state;

  using agent_ref_t = samlib::base_agent<T>;

  agent_ref_t* output;

  Task producer;

  ping_pong_agent()
    : state{nullptr}
  { }

  ping_pong_agent(State& gstate, Task fn)
    : state(&gstate),
      producer(fn)
  { }

  ~ping_pong_agent() { }

  void set_output(agent_ref_t& out)
  {
    output = &out;
  }

  void run(std::stop_token stoken)
  {
    while (!stoken.stop_requested()) {
      auto data = this->receive();
      auto new_data = producer(*data);
      output->send(*new_data);
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
  
  using ping_t = ping_pong_agent<state,double,ping>;
  using pong_t = ping_pong_agent<state,double,pong>;

  using ref_ping_t = samlib::agent_ref<ping_t>;
  using ref_pong_t = samlib::agent_ref<pong_t>;

  ref_ping_t p1(new ping_t(st, ping()));
  ref_pong_t p2(new pong_t(st, pong()));

  p1.ref_agent().set_output(p2.ref_agent());
  p2.ref_agent().set_output(p1.ref_agent());

  p1.start();
  p2.start();

  sleep(1);

  p2.send(1);

  sleep(1);

  printf("------------ Time's up ---------------\n");

  p1.stop();
  p2.stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

}
