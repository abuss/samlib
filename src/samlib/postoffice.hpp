#pragma once

#include <vector>
#include "channel.hpp"

namespace samlib
{

struct postoffice
{
  std::vector<channel_base*> mboxes;

  ~postoffice()
  {
    for(auto ch : mboxes)
      delete ch;
  }

  template<typename T>
  channel<T>& make_channel() 
  {
    channel<T>* tmp = new channel<T>();
    mboxes.push_back(tmp);
    return *tmp; 
  }

  size_t total_to_deliver() const 
  {
    size_t sum = 0;
    for(const auto& ch : mboxes)
      sum += ch->size();
    return sum;
  }

  void close()
  {
    for(auto& ch : mboxes)
      ch->close();
  }
};

} // namespace samlib
