#include <unistd.h>
#include <iostream>
#include <samlib/base_agent.hpp>
#include <samlib/environment.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

struct my_stateless_agent
  : public samlib::base_agent<double>
{
  template<typename Env>
  my_stateless_agent(Env& env)
    : samlib::base_agent<double>{env.template make_channel<double>()}
  { }

  void run(const std::stop_token&)
  {
    auto dat = this->try_receive();
    CHECK(dat == std::nullopt);
    
    dat = this->receive();
    CHECK(dat == 1.234);

    dat = this->try_receive();
    CHECK(dat == 5.67);

    dat = this->try_receive();
    CHECK(dat == std::nullopt);
  }
};


TEST_CASE("stateless agent") {

  {
    using env_t = samlib::environment;

    env_t env;

    auto a = env.create_agent<my_stateless_agent>();

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    using env_t = samlib::environment;

    env_t env;

    env.create_agent<my_stateless_agent>();
    auto a = env.get_agent_ref<double>("_1");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    using env_t = samlib::environment;

    env_t env;

    auto b = env.create_agent_named<my_stateless_agent>("agent_a");
    auto a = env.get_agent_ref<double>("agent_a");

    sleep(1);
    b.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }
}


// Stateful agent
template<typename Env, typename S>
struct my_agent
  : public samlib::base_agent<double>,
    public S

{
  Env& env;

  my_agent(Env& e)
    : env(e)
  { }

  void run(const std::stop_token&) override
  {
    auto dat = this->try_receive();
    CHECK(dat == std::nullopt);
    
    dat = this->receive();
    CHECK(dat == this->res1);

    dat = this->try_receive();
    CHECK(dat == this->res2);

    dat = this->try_receive();
    CHECK(dat == std::nullopt);

    CHECK(this->res3 == 0);
    this->res3 = (this->res1 + this->res2);
  }
};


TEST_CASE("stateful agent") {

  {
    struct my_state
    {
      double res1 = 1.234;
      double res2 = 5.67;
      double res3 = 0;
    };
  
    using env_t = samlib::environment;
    using agent_t = my_agent<env_t, my_state>;

    env_t env;

    auto a = env.create_agent_named<agent_t>("agent_a");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    auto res3 = samlib::template state_of<agent_t>(a).res3;
    CHECK(res3 == 6.904);
 
    CHECK(true);
  }

  {
    struct my_state
    {
      double res1 = 1.234;
      double res2 = 5.67;
      double res3 = 0;
    };
  
    using env_t = samlib::environment;

    env_t env;

    env.create_agent<my_agent<env_t, my_state>>();
    auto a = env.get_agent_ref<double>("_1");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    struct my_state
    {
      double res1 = 1.234;
      double res2 = 5.67;
      double res3 = 0;
    };
  
    using env_t = samlib::environment;

    env_t env;

    auto b = env.create_agent_named<my_agent<env_t, my_state>>("agent_a");
    auto a = env.get_agent_ref<double>("agent_a");

    sleep(1);
    b.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }
}
