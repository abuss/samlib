#include <unistd.h>
#include <iostream>

#include <samlib/base_agent.hpp>
#include <samlib/runtime.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


// Environment global state agent
template<typename Env>
struct my_agent
  : public samlib::base_agent<double>
{
  Env& env;

  explicit my_agent(Env& e) : env(e) {};

  void run(const std::stop_token&) override
  {
   
    auto dat = this->receive();
    CHECK(dat == env.res1);

    dat = this->try_receive();
    CHECK(dat == env.res2);

    CHECK(env.res3 == 0);
    env.res3 = (env.res1 + env.res2);
  }
};


TEST_CASE("external state") {

  {
    struct my_environment
      : public samlib::runtime<my_environment>
    {
      double res1 = 1.234;
      double res2 = 5.67;
      double res3 = 0;
    };
  
    using env_t = my_environment;
    using agent_t = my_agent<env_t>;

    env_t env;

    auto a = env.create_agent<agent_t>();

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(env.res3 == 6.904);
  }

}
