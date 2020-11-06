#include <unistd.h>
#include <iostream>
#include <samlib/base_agent.hpp>
#include <samlib/mailbox.hpp>
#include <samlib/environment.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

struct my_stateless_agent
  : public samlib::base_agent<double>
{
  my_stateless_agent() {};

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


// Stateful agent
template<typename S>
struct my_agent
  : public samlib::base_agent<double>
{
  S* env;

  my_agent(S& e) : env(&e) {};

  void run(const std::stop_token&) override
  {
    auto dat = this->try_receive();
    CHECK(dat == std::nullopt);
    
    dat = this->receive();
    CHECK(dat == env->state().res1);

    dat = this->try_receive();
    CHECK(dat == env->state().res2);

    dat = this->try_receive();
    CHECK(dat == std::nullopt);
  }
};


TEST_CASE("stateless agent") {

  {
    using env_t = samlib::environment<>;

    env_t env;

    auto a = env.create_stateless_agent<my_stateless_agent>();

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    using env_t = samlib::environment<>;

    env_t env;

    env.create_stateless_agent<my_stateless_agent>();
    auto a = env.get_agent_ref<double>("_1");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    using env_t = samlib::environment<>;

    env_t env;

    auto b = env.create_stateless_agent_named<my_stateless_agent>("agent_a");
    auto a = env.get_agent_ref<double>("agent_a");

    sleep(1);
    b.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }
}

TEST_CASE("stateful agent") {

  {
    struct state {
      double res1 = 1.234;
      double res2 = 5.67;
    };
  
    using env_t = samlib::environment<state>;

    env_t env;

    auto a = env.create_statefull_agent_named<my_agent<env_t>>("agent_a");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    struct state {
      double res1 = 1.234;
      double res2 = 5.67;
    };
  
    using env_t = samlib::environment<state>;

    env_t env;

    env.create_statefull_agent<my_agent<env_t>>();
    auto a = env.get_agent_ref<double>("_1");

    sleep(1);
    a.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

  {
    struct state {
      double res1 = 1.234;
      double res2 = 5.67;
    };
  
    using env_t = samlib::environment<state>;

    env_t env;

    auto b = env.create_statefull_agent_named<my_agent<env_t>>("agent_a");
    auto a = env.get_agent_ref<double>("agent_a");

    sleep(1);
    b.send(1.234);
    a.send(5.67);
    sleep(1);
    env.stop_agents();

    CHECK(true);
  }

}
