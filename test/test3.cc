#include <unistd.h>
#include <iostream>
#include <samlib/agent.hpp>
#include <samlib/environment.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


// Stateful agent
template<typename S>
struct my_agent
  : public samlib::base_agent<double>
{
  S* env;

  my_agent(S& e) : env(&e) {};

  void run(std::stop_token)
  {
   
    auto dat = this->receive();
    CHECK(dat == env->get_state().res1);

    dat = this->try_receive();
    CHECK(dat == env->get_state().res2);

    CHECK(env->state().res3 == 0);
    env->state().res3 = (env->get_state().res1 + env->get_state().res2);
  }
};


TEST_CASE("external state") {

  {
    struct state {
      double res1 = 1.234;
      double res2 = 5.67;
      double res3 = 0;
    };
  
    using env_t = samlib::environment<state>;

    state my_state;
    env_t env(my_state);

    auto a = env.create_agent<my_agent<env_t>>();

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    my_state = env.get_state();
    CHECK(my_state.res3 == 6.904);
  }

}
