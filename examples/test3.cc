
#include <unistd.h>
#include <iostream>

#include <samlib/agent.hpp>
#include <samlib/agent_ref.hpp>


template<typename State, typename Tin, typename Tout=Tin>
struct ping_pong_agent
  : public samlib::agent<State, Tin>
{
  using base_t = samlib::agent<State, Tin>;
  using task_t = std::function<Tout(Tin)>;
  using agent_ref_type = samlib::agent_ref<ping_pong_agent>;

  using dest_agent_t = samlib::agent_ref<ping_pong_agent<State, Tin, Tout>>;

  task_t        task;
  dest_agent_t& out;

  constexpr ping_pong_agent(State& state, task_t&& fn, dest_agent_t& d)
  : base_t{state},
    task{fn},
    out(d)
  { }

  constexpr agent_ref_type ref() noexcept
  {
    return agent_ref_type(this);
  }

  void run(std::stop_token st) override
  {
    while (!st.stop_requested()) {
      auto data = this->receive();
      auto new_data = task(data);
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
  samlib::empty_state st;

  using agent_t = ping_pong_agent<samlib::empty_state, double>;
  using agent_ref_t = agent_t::agent_ref_type;

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

  p1.stop();
  p2.stop();

}
