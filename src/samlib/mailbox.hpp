// Mailbox implementation using queues
#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#ifndef NO_CONCURRENT_QUEUE
#include "concurrentqueue.h"
#else
#include <queue>
#endif

namespace samlib
{
 
  template<typename T>
  class mailbox
#ifndef NO_CONCURRENT_QUEUE
    : moodycamel::ConcurrentQueue<T>
#else
    : std::queue<T>
#endif
  {
  public:

    typedef T value_type;

#ifndef NO_CONCURRENT_QUEUE
    
    using moodycamel::ConcurrentQueue<T>::ConcurrentQueue;
    using moodycamel::ConcurrentQueue<T>::enqueue;
    using moodycamel::ConcurrentQueue<T>::try_dequeue;
    using moodycamel::ConcurrentQueue<T>::size_approx;

#else

    using std::queue<T>::queue;

    bool enqueue(T const& value)
    {
      this->emplace(value);
      return true;
    }


    bool try_dequeue(T& value)
    {
      value = T();
      if (!this->empty()) {
        value = this->front();
        this->pop();
        return true;
      }
      return false;
    }


    size_t size_approx() const
    {
      return this->size();
    }

#endif


    bool send(value_type const& value)
    {
      return this->enqueue(value);
    }

    bool send(value_type&& value)
    {
      return this->enqueue(std::move(value));
    }

    value_type receive()
    {
      value_type value;
      while (!this->try_dequeue(value)) {
        std::this_thread::yield();
      }
      return std::move(value);
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (this->try_dequeue(value)) {
        return std::optional<value_type>{std::move(value)};
      }
      return std::nullopt;
    }

  };

}
