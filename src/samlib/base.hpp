#pragma once

#include <thread>
#include <functional>

#include <samlib/mailbox.hpp>

namespace samlib
{

class executor
{
  std::thread   executor_thread;

public:

  virtual void run() = 0;

  void start()
  {
    executor_thread = std::thread([&] { run(); });
  }

  void wait()
  {
    if (executor_thread.joinable())
      executor_thread.join();
  }

  void detach()
  {
    executor_thread.detach();
  }

};


template <typename T>
class base
  : public executor
{
  typedef mailbox<T>  mailbox_type;

  mailbox_type  _mbox;

protected:

  T receive()
  {
    return std::move(_mbox.receive());
  }

  std::optional<T> try_receive()
  {
    return _mbox.try_receive();
  }

public:
  mailbox_type& mbox()
  {
    return _mbox;
  }

  mailbox_type *ptr_mbox()
  {
    return &_mbox;
  }

  bool send(const T &value)
  {
    return _mbox.send(value);
  }

  bool send(T &&value)
  {
    return _mbox.send(std::move(value));
  }
};

} // namespace samlib
