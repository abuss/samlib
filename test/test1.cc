#include <unistd.h>
#include <iostream>
#include <samlib/base.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


TEST_CASE("testing mailbox") {

  samlib::mailbox<double> mout;

  CHECK(mout.size_approx() == 0);

  mout.send(3.14);

  CHECK(mout.size_approx() == 1);

  CHECK(mout.receive() == 3.14);
  CHECK(mout.size_approx() == 0);
}

TEST_CASE("testing mailbox's agent") {

  struct my_agent
    : public samlib::base<double>
  {
    void run()
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

  my_agent a;
  a.start();
  sleep(1);
  a.send(1.234);
  a.send(5.67);
  sleep(1);
  a.wait();

  CHECK(true);
}
