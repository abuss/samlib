
#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>
#include <samlib/agent_ref.hpp>


template<typename GS, typename Tin, typename Tout=Tin>
struct ping_pong_agent
  : public samlib::agent<GS,Tin>
{
  typedef samlib::agent<GS,Tin>                    base_t;
  typedef std::function<Tout(Tin)>                 task_t;
  typedef samlib::agent_ref<ping_pong_agent>       agent_ref_type;

  typedef samlib::agent_ref<ping_pong_agent<GS,Tin,Tout>> dest_agent_t;

  using base_t::global_state;
  using typename base_t::agent;  // using namespace std::literals::chrono_literals;


  task_t task;
  dest_agent_t& out;

  ping_pong_agent(GS& state, task_t&& fn, dest_agent_t& d)
  : base_t{state},
    task{fn},
    out(d)
  { }

  agent_ref_type ref()
  {
    return agent_ref_type(this);
  }

  void run()
  {
    while (!global_state->terminate) {
      Tin data = this->receive();
      Tout new_data = task(data);
      out.send(new_data);
    }
  }
};


double ping(double val)
{
    printf("Ping -> %f\n",val++);
    return val;
}


double pong(double val)
{
    printf("%f <- Pong\n",val++);
    return val;
}


int main()
{
  struct state { bool terminate = false; };
  state st;

  typedef ping_pong_agent<state,double> agent_t;
  typedef agent_t::agent_ref_type agent_ref_t;

  agent_ref_t p1,p2;

  auto pp1 = agent_t(st, ping, p2);
  auto pp2 = agent_t(st, pong, p1);

  p1 = pp1.ref();
  p2 = pp2.ref();

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
