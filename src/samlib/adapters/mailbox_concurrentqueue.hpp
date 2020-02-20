// Mailbox adapter to use moodycamel::concurrentqueue
#pragma once

#include <iostream>
#include <optional>
#include <chrono>
#include <jthread.hpp>

#include "concurrentqueue.h"

namespace samlib
{
 
  using namespace std::literals::chrono_literals;

  template<typename T>
  class mailbox
    : moodycamel::ConcurrentQueue<T>
  {

    std::stop_token stop_flag;

  public:

    typedef T value_type;
    
    using moodycamel::ConcurrentQueue<T>::ConcurrentQueue;
    using moodycamel::ConcurrentQueue<T>::enqueue;
    using moodycamel::ConcurrentQueue<T>::try_dequeue;
    using moodycamel::ConcurrentQueue<T>::size_approx;

    void set_stop_token(std::stop_token st)
    {
      stop_flag = st; 
    }

    bool send(const value_type& value)
    {
      if (!stop_flag.stop_requested())
        return this->enqueue(value);
      return false;
    }

    bool send(value_type&& value)
    {
      if (!stop_flag.stop_requested())
        return this->enqueue(std::forward<value_type>(value));
      return false;
    }

    std::optional<value_type> receive()
    {
      value_type value;
      while (!this->try_dequeue(value)) {
        std::this_thread::sleep_for(5ms);
        if (stop_flag.stop_requested())
          return std::nullopt;
      }
      // return value;
      return std::make_optional(std::move(value));
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (this->try_dequeue_non_interleaved(value) && !stop_flag.stop_requested())
        return std::make_optional(std::move(value));
      return std::nullopt;
    }

    void close() { }


  };

}
