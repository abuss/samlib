// Mailbox adapter to use MPMCqueue
#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#include <include/rigtorp/MPMCQueue.h>


namespace samlib
{

template<typename T>
class mailbox
  : public rigtorp::MPMCQueue<T>

{

  using base_t = rigtorp::MPMCQueue<T>;

  bool enqueue(T const& value)
  {
    this->push(value);
    return true;
  }


  bool try_dequeue(T& value)
  {
    this->pop(value);
    return true;
  }

  bool dequeue(T& value)
  {
    this->pop(value);
    return true;
  }

public:
  mailbox()
    : base_t(8)
  { }

  size_t size_approx() const
  {
    return static_cast<int>(this->head_.load(std::memory_order_release) - this->tail_.load(std::memory_order_release));
  }


  typedef T value_type;

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
    this->dequeue(value);
    return std::make_optional(std::move(value));
  }
};

}
