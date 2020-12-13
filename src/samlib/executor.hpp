#pragma once

#include <thread>

namespace samlib
{

class executor: public std::jthread 
{ 
  using std::jthread::jthread;
};

} // namespace samlib
