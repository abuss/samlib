#pragma once

#include <functional>

#include <samlib/agent_worker.hpp>
#include <samlib/channel.hpp>

namespace samlib
{

template<typename T>
class base_agent
  : public agent_worker
{
public:

  inline std::optional<T> receive()
  {
    return mailbox.receive();
  }

  inline std::optional<T> try_receive()
  {
    return mailbox.try_receive();
  }

  using mailbox_type = channel<T>;
  using message_type = T;

  base_agent()
    : mailbox{*(new channel<T>())},
      destroy_mailbox{true} 
  { }

  base_agent(mailbox_type& chn) : mailbox{chn}
  { }

  virtual ~base_agent()
  {
    stop();
    if (destroy_mailbox)
      delete &mailbox;
  }

  void stop()
  {
    mailbox.close();
    agent_worker::stop();
  }

  bool send(const T& value)
  {
    return mailbox.send(value);
  }

  bool send(T&& value)
  {
    return mailbox.send(std::forward<T>(value));
  }

protected:
  mailbox_type&  mailbox;

private:
  bool destroy_mailbox = false;

};

} // namespace samlib
