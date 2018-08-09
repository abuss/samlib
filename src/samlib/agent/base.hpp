#pragma once

#include <thread>
#include <functional>

#include <samlib/mailbox.hpp>

namespace samlib
{

template <typename T>
class agent_base
{
  typedef samlib::mailbox<T>  mailbox_type;

  mailbox_type _mbox;

protected:
  std::thread executor;

  T receive()
  {
    return std::move(_mbox.receive());
  }

  std::optional<T> try_receive()
  {
    return _mbox.try_receive();
  }

  agent_base &operator=(const agent_base &)
  {
    return *this;
  }

  virtual ~agent_base() = default;

  virtual void run() = 0;

public:
  mailbox_type &mbox()
  {
    return _mbox;
  }

  mailbox_type *ptr_mbox()
  {
    return &_mbox;
  }

  void start()
  {
    executor = std::thread([&] { run(); });
  }

  void wait()
  {
    if (executor.joinable())
      executor.join();
  }

  void detach()
  {
    executor.detach();
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
