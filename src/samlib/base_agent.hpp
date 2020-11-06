#pragma once

// #include <jthread.hpp>
#include <functional>

#include <samlib/agent_worker.hpp>
#include <samlib/mailbox.hpp>

namespace samlib
{

template<typename T>
class base_agent
  : public agent_worker
{
protected:
  inline std::optional<T> receive()
  {
    return _mbox.receive();
  }

  inline std::optional<T> try_receive()
  {
    return _mbox.try_receive();
  }

public:
  using mailbox_type = mailbox<T>;
  using message_type = T;

  virtual ~base_agent()
  {
    stop();
  }

  void stop()
  {
    _mbox.close();
    agent_worker::stop();
  }

  bool send(const T& value)
  {
    return _mbox.send(value);
  }

  bool send(T&& value)
  {
    return _mbox.send(std::forward<T>(value));
  }

  mailbox_type& mbox()
  {
    return _mbox;
  }

private:
  mailbox_type  _mbox;
};

} // namespace samlib
