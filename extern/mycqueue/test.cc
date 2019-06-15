#include <iostream>

#include "mycqueue.hpp"


int main(int argc, char const *argv[])
{
  mycqueue<int> q(10);

  q.push(10);
  std::cout << "q.size:" << q.approx_size() << std::endl;
  
  auto a = q.pop();
  std::cout << "q.size:" << q.approx_size() << std::endl;
  std::cout << "Value:" << a << std::endl;

  int c;
  if (q.try_pop(c)) {
    std::cout << "q.size:" << q.approx_size() << std::endl;
    std::cout << "Value:" << c << std::endl;
  } else {
    std::cout << "No elements\n";
  }
  
  q.push(20);
  
  auto b = q.pop();
  std::cout << "q.size:" << q.approx_size() << std::endl;
  std::cout << "Value:" << b << std::endl;


  return 0;
}
