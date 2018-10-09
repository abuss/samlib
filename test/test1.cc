#include <unistd.h>
#include <iostream>
#include <samlib/base.hpp>


struct my_agent
  : public samlib::base<double>
{
  void run()
  {
    std::cout << "Hello World\n";
    auto dat = this->try_receive();
    if (dat)
      std::cout << "REceived: " << dat.value_or(0.0) << std::endl;
    else
      std::cout << "Nothing\n";
    auto dat2 = this->receive();
    if (dat2)
      std::cout << "REceived: " << dat2 << std::endl;
    else
      std::cout << "Nothing\n";
  }
};

struct generator
{
  int n;

  generator(int _n)
    : n(_n)
  { }

  std::optional<double> operator()()
  {
    if (n>0)
      return n--*2.3;
    return std::nullopt;
  }
};

int main()
{
  std::cout << "Hello, from samlib!\n";
  my_agent a;
  a.start();
  sleep(1);
  a.send(3.124);
  sleep(1);
  a.wait();

  generator g(10);
  std::cout << *g() << std::endl;
  std::cout << *g() << std::endl;
  std::cout << *g() << std::endl;

  struct state{ bool terminate; };
  // state st;
  samlib::mailbox<double> mout;
  return 0;
}
