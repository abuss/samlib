#pragma once

// #include <jthread.hpp>
#include <functional>

#include <samlib/executor.hpp>
#include <samlib/mailbox.hpp>
#include <samlib/agent_ref.hpp>

namespace samlib
{

  template <typename T>
  class base_agent
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
    using agent_ref_type = agent_ref<base_agent>;
    // using executor_type = std::jthread;
    using executor_type = executor;
    using mailbox_type = mailbox<T>;

    // base_agent() = default;
    base_agent() {};

    ~base_agent() {
      stop();
    }

    void start(uint num_workers = 1)
    {
      // _executor = std::jthread([&](std::stop_token st) {
          // _mbox.set_stop_token(st); 
          // run(st); 
        // });
      _executor = executor([&](std::stop_token st) {
          _mbox.set_stop_token(st); 
          run(st); 
        }, num_workers);
    }

    void stop()
    {
      _mbox.close();
      _executor.request_stop();
    }

    bool send(const T& value)
    {
      return _mbox.send(value);
    }

    // bool send(T value)
    // {
    //   return _mbox.send(value);
    // }

    bool send(T&& value)
    {
      return _mbox.send(std::forward<T>(value));
    }

    agent_ref_type ref() noexcept
    {
      return agent_ref_type(this);
    }

    executor_type* get_executor() {
      return &_executor;
    }

    mailbox_type& mbox()
    {
      return _mbox;
    }

    virtual void run(std::stop_token st) = 0;

  private:

    executor_type  _executor;
    mailbox_type   _mbox;

  };

} // namespace samlib
