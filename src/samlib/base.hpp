#pragma once

#include <thread>
#include <functional>

#include <samlib/mailbox.hpp>

namespace samlib
{

  class executor
  {
    std::vector<std::thread>   executor_threads;

  public:

    virtual ~executor() {}

    virtual void run() = 0;

    void start(u_int n=1)
    {
      for (u_int i=0;i<n;++i)
        executor_threads.emplace_back(std::thread([&] { run(); }));
    }

    void wait()
    {
      for (auto& thrd : executor_threads)
        if (thrd.joinable())
          thrd.join();
    }

    void detach()
    {
      for (auto& thrd : executor_threads)
        thrd.detach();
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
      return _mbox.receive();
    }

    inline std::optional<T> try_receive()
    {
      return _mbox.try_receive();
    }

  public:
    mailbox_type& mbox()
    {
      return _mbox;
    }


    bool send(const T& value)
    {
      return _mbox.send(value);
    }

    bool send(T&& value)
    {
      return _mbox.send(std::forward<T>(value));
    }
  };

} // namespace samlib
