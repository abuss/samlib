// Mailbox adapter to use moodycamel::concurrentqueue
#pragma once

#include <iostream>
#include <optional>
#include <chrono>
#include <thread>

#include "concurrentqueue.h"

namespace samlib
{

using namespace std::literals::chrono_literals;

template<typename T>
class mpmc_queue
  : moodycamel::ConcurrentQueue<T>
{

  std::atomic_bool stop_flag = false;

public:
  typedef T value_type;

  using moodycamel::ConcurrentQueue<T>::ConcurrentQueue;
  using moodycamel::ConcurrentQueue<T>::enqueue;
  using moodycamel::ConcurrentQueue<T>::try_dequeue;
  using moodycamel::ConcurrentQueue<T>::size_approx;

  void stop(bool flag)
  {
    stop_flag = flag;
  }

  bool send(const value_type& value)
  {
    if (!stop_flag)
      return this->enqueue(value);
    return false;
  }

  bool send(value_type&& value)
  {
    if (!stop_flag)
      return this->enqueue(std::forward<value_type>(value));
    return false;
  }

  std::optional<value_type> receive()
  {
    value_type value;
    while (!this->try_dequeue(value)) {
      std::this_thread::sleep_for(10us);
      if (stop_flag)
        return std::nullopt;
    }
    // return value;
    return std::make_optional(std::move(value));
  }

  std::optional<value_type> try_receive()
  {
    value_type value;
    if (!stop_flag && this->try_dequeue_non_interleaved(value))
      return std::make_optional(std::move(value));
    return std::nullopt;
  }

  void close()
  {
    stop_flag = true;
  }
};

}
