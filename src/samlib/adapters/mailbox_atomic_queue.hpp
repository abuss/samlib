// Mailbox adapter to use atomic_queue

#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#include "atomic_queue.h"


// ////////////////////////////////////////
namespace atomic_queue
{

template<typename T, unsigned SIZE, bool MinimizeContention = details::IsPowerOf2<SIZE>::value>
class MyAtomicQueue
  : public AtomicQueueCommon<MyAtomicQueue<T, SIZE>>
{
  alignas(CACHE_LINE_SIZE) std::atomic<T> elements_[SIZE] = {}; // Empty elements are NIL.
  T NIL = 0.0;

  friend class AtomicQueueCommon<MyAtomicQueue<T, SIZE>>;

  static constexpr auto size = SIZE;
  using Remap = typename details::GetIndexShuffleBits<MinimizeContention, SIZE, CACHE_LINE_SIZE / sizeof(T)>::type;

  T do_pop(unsigned tail) noexcept
  {
    unsigned index = details::remap_index(tail % SIZE, Remap{});
    for (;;) {
      T element = elements_[index].exchange(NIL, R);
      if (element != NIL)
        return element;
      /*_mm_pause()*/;
    }
  }

  void do_push(T element, unsigned head) noexcept
  {
    assert(element != NIL);
    unsigned index = details::remap_index(head % SIZE, Remap{});
    for (T expected = NIL; !elements_[index].compare_exchange_strong(expected, element, R, X); expected = NIL) // (1) Wait for store (2) to complete.
      /*_mm_pause()*/;
  }

public:
  using value_type = T;

  MyAtomicQueue() noexcept
  {
    assert(std::atomic<T>{NIL}.is_lock_free());
    if (T{} != NIL)
      for (auto& element : elements_)
        element.store(NIL, X);
  }
};

}
// ////////////////////////////////////////

namespace samlib
{

template<typename T>
class mailbox
  : public atomic_queue::MyAtomicQueue<T, 8>
{

  using base_t = atomic_queue::MyAtomicQueue<T, 8>;

  bool enqueue(T const& value)
  {
    this->push(value);
    return true;
  }


  bool try_dequeue(T& value)
  {
    value = this->pop();
    return true;
  }

public:
  size_t size_approx() const
  {
    return static_cast<int>(this->head_.load(atomic_queue::X) - this->tail_.load(atomic_queue::X));
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
    // if (this->try_dequeue_non_interleaved(value))
    this->try_dequeue(value);
    return std::make_optional(std::move(value));
    // return std::nullopt;
  }

  void close() { }
  bool is_closed()
  {
    return false;
  }
};

}
