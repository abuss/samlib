// Mailbox adapter to use moodycamel::concurrentqueue
#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#include "concurrentqueue.h"

namespace samlib
{
 
  template<typename T>
  class mailbox
    : moodycamel::ConcurrentQueue<T>
  {
  public:

    typedef T value_type;
    
    using moodycamel::ConcurrentQueue<T>::ConcurrentQueue;
    using moodycamel::ConcurrentQueue<T>::enqueue;
    using moodycamel::ConcurrentQueue<T>::try_dequeue;
    using moodycamel::ConcurrentQueue<T>::size_approx;


    bool send(const value_type& value)
    {
      return this->enqueue(value);
    }

    bool send(value_type&& value)
    {
      return this->enqueue(std::forward<value_type>(value));
    }

    value_type receive()
    {
      value_type value;
      while (!this->try_dequeue(value)) {
        std::this_thread::yield();
      }
      return value;
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (this->try_dequeue_non_interleaved(value))
        return std::make_optional(std::move(value));
      return std::nullopt;
    }

    void close() { }


  };

}
