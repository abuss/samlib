#pragma once

#include "mpmc_queue.hpp"

namespace samlib
{

struct channel_base
{
  virtual ~channel_base() { }
  virtual size_t size() = 0;
  virtual void close() = 0;
};


template<typename T>
class channel 
  : public channel_base
{
  using mailbox_type = mpmc_queue<T>;

  mailbox_type queue;

public:

  using message_type = T;

  size_t size() { return queue.size_approx(); }

  bool send(const message_type& value)
  {
    return queue.send(value);
  }

  bool send(message_type&& value)
  {
    return queue.send(std::forward<message_type>(value));
  }

  std::optional<message_type> receive()
  {
    return queue.receive();
  }

  std::optional<message_type> try_receive()
  {
    return queue.try_receive();
  }

  void close()
  {
    queue.close();
  }
};

} // namespace samlib
