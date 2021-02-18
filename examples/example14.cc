
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <queue>

/*
  (G)-->(P)->(p1)--+-->(S)
         |         |
         +-->(p2)--+
         |         |
         +-->(p3)--+ 
*/

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>
#include <samlib/statefull_agent.hpp>
#include <samlib/mpmc_queue.hpp>

auto my_generator = [](auto fn, auto& out) {
  using namespace std::literals::chrono_literals;

  return [fn, &out](auto& in_port) {
    size_t sum = 0;
    if (auto data = in_port.receive()) {
      auto n = *data;
      while ((n > 0)) {
        auto val = fn(n);
        sum += val;
        out.send(val);
        --n;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    }
    printf("Sum: %lu\n", sum);
  };
};

template<typename Fn, typename O>
auto my_transform(Fn fn, O& out)
{
  return [fn, &out](auto&, auto& in_port) {
    // printf(" ++ Inside transform,queue size:%ld\n", in_port.size());
    auto data = in_port.receive();
    // printf(" ++ Inside transform, data:%d\n",*data);
    out.send(fn(std::move(*data)));
  };
}

auto signal_task(auto task, auto* free_workers, int wid) {
  return [=](auto& env, auto& in_mbox) {
      printf("/-------^Task %d started queue.size: %lu\n", wid, in_mbox.size());
      task(env, in_mbox);
      printf("\\-------.Task %d ended\n", wid);
      free_workers->enqueue(wid);
    };
}

template<typename Env, typename Tin>
class processing_agent
  : public samlib::statefull_agent<Env,Tin>
{
  using base_t = samlib::statefull_agent<Env,Tin>;
  using worker_ref_t = samlib::agent_ref<Tin>;

  std::vector<worker_ref_t>  workers;
  samlib::mpmc_queue<int>    free_workers;

public:
  using base_t::base_t;

  void run(const std::stop_token& st) override
  {
    workers.push_back( this->environment.template make_statefull_agent<Tin>(signal_task(this->task, &free_workers, 0)) );
    free_workers.enqueue(0);

    workers.push_back( this->environment.template make_statefull_agent<Tin>(signal_task(this->task, &free_workers, 1)) );
    free_workers.enqueue(1);

    workers.push_back( this->environment.template make_statefull_agent<Tin>(signal_task(this->task, &free_workers, 2)) );
    free_workers.enqueue(2);

    while (!st.stop_requested()) {
      if (free_workers.size_approx()>0 and !st.stop_requested()) {
        auto data = this->mailbox.receive();
        auto free_wid = free_workers.receive();
        printf("--------- Next wid: %d\n",*free_wid);
        // printf("Found free worker\n");
        workers[*free_wid].send(*data);
      }
    }

  }
};


void hole(int val)
{
  printf("To the hole -> %d\n", val);
}

int peek(int val)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100+(val%3)*100));
  printf(">> Value in transit -> %d\n", val);
  return val;
}


int gen(int val)
{
  printf("Generate -> %d\n", ++val);
  return val;
}


struct engine
{
  using env_t = samlib::environment<>;

  env_t env;
  samlib::agent_ref<int> p1, p2, p3;

  template<typename Gen, typename Sink>
  engine(Gen _generator, Sink _sink)
  {
    p1 = env.make_stateless_agent<int>(my_generator(_generator, p2));
    p2 = env.create_statefull_agent<processing_agent<env_t,int>>(my_transform(peek,p3));
    p3 = env.make_statefull_agent<int>(samlib::sink(_sink));
  }

  void run_for(size_t n, u_int t)
  {
    auto p1 = env.template get_agent_ref<int>("_1");
  
    p1.send(n);

    sleep(t);

    printf("------------ Time's up ---------------\n");

    env.stop_agents();

    printf("------------ Clean up ---------------\n");
  }
};


int main()
{
  auto eng = engine(gen, hole);
  eng.run_for(100, 2);
}
