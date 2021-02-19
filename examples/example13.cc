
#include <unistd.h>
#include <iostream>

/*
  (G)-->(P)->(p1)--+-->(S)
         |         |
         +-->(p2)--+
         |         |
         +-->(p3)--+ 
*/

#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>
#include <samlib/agent.hpp>


auto my_generator = [](auto fn, auto& out) {
  using namespace std::literals::chrono_literals;

  return [fn, &out](auto& agent) {
    size_t sum = 0;
    if (auto data = agent.receive()) {
      auto n = *data;
      while ((n > 0)) {
        auto val = fn(n);
        sum += val;
        out.send(val);
        --n;
      }
    }
    printf("Sum: %lu\n", sum);
  };
};

template<typename Env, typename Tin>
class processing_agent
  : public samlib::agent<Env,Tin>
{
  using base_t = samlib::agent<Env,Tin>;
  using out_t = samlib::agent_ref<Tin>;

  std::vector<out_t> workers;

public:
  using base_t::base_t;

  void run(const std::stop_token& st) override
  {
    workers.push_back( this->environment.template make_agent<Tin>(this->task) );
    workers.push_back( this->environment.template make_agent<Tin>(this->task) );
    workers.push_back( this->environment.template make_agent<Tin>(this->task) );
    size_t i = 0;
    while (!st.stop_requested()) {
      if (auto data = this->mailbox.try_receive()) {
        printf("Proc queue.size: %lu\n",this->mailbox.size());
        workers[i].send(*data * (2*i-1));
        i = (i+1) % workers.size();
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
  sleep(1);
  printf("Value in transit -> %d\n", val);
  return val;
}


int gen(int val)
{
  printf("Generate -> %d\n", ++val);
  return val;
}


struct engine
{
  using env_t = samlib::environment;

  env_t env;
  samlib::agent_ref<int> p1, p2, p3;

  template<typename Gen, typename Sink>
  engine(Gen _generator, Sink _sink)
  {
    p1 = env.make_agent<int>(my_generator(_generator, p2));
    p2 = env.create_agent<processing_agent<env_t,int>>(samlib::transform(peek,p3));
    p3 = env.make_agent<int>(samlib::sink(_sink));
  }

  void run_for(size_t n, u_int t)
  {
    auto p1 = env.get_agent_ref<int>("_1");
  
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
  eng.run_for(100, 10);
}
