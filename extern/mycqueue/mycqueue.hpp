#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <emmintrin.h>

template<typename T>
class mycqueue
{
  using turns_t = std::atomic<uint8_t>;

  static constexpr size_t cache_line_size = 64;
  const size_t capacity;
  bool closed = false;
  turns_t* turns = nullptr;
  T* elements = nullptr;

  const uint8_t USED = 1;
  const uint8_t UNUSED = 0;


protected:
  alignas(cache_line_size) std::atomic<size_t> head = {};
  alignas(cache_line_size) std::atomic<size_t> tail = {};

public:
  
  mycqueue(size_t capacity_)
    : capacity{capacity_}
  { 
    closed = false;
    turns = new(std::align_val_t{cache_line_size}) turns_t[capacity];
    elements = new(std::align_val_t{alignof(T)}) T[capacity];
  }

  ~mycqueue()
  {
    closed = true;
    delete [] turns;
    delete [] elements;
  }


  void push(T value) noexcept 
  { 
    auto const head_ = head.fetch_add(1);
    auto idx = index(head_);
    while (turns[idx].load(std::memory_order_acquire) != UNUSED) {
      if (closed) return;
      // _mm_pause();
    }
    // printf("push %d : %f\n",idx,value);
    elements[idx] = std::move(value);
    turns[idx].store(USED, std::memory_order_release);
  }


  // bool try_push(T value) noexcept
  // {
  //   // printf("try_push 1\n");
  //   auto head_ = head.load(std::memory_order_acquire);
  //   do {
  //     if (closed) return false;
  //     if (head_ - tail.load(std::memory_order_relaxed) >= capacity)
  //       return false;
  //   } while (!head.compare_exchange_strong(head_, head_ + 1, 
  //             std::memory_order_acquire, std::memory_order_relaxed));
  //   auto idx = index(head_);
  //   elements[idx] = std::move(value);
  //   turns[idx].store(turn(head_)*2+1, std::memory_order_relaxed);
  //   return true;
  // }


  // void pop(T& value) noexcept
  // { 
  //   // printf("pop 1\n");
  //   auto const tail_ = tail.fetch_add(1);
  //   auto idx = index(tail_);
  //   while (turns[idx].load(std::memory_order_acquire) != USED) {
  //     if (closed) {
  //       value = std::move(elements[idx]);    
  //       return;
  //     }
  //   }
  //   value = std::move(elements[idx]);
  //   turns[idx].store(UNUSED, std::memory_order_release);
  // }


  T pop() noexcept
  { 
    // printf("pop 2\n");
    auto const tail_ = tail.fetch_add(1);
    auto idx = index(tail_);
    while (turns[idx].load(std::memory_order_acquire) != USED) {
      if (closed) { 
        return elements[idx];
      }
      // _mm_pause();
    }
    T v = std::move(elements[idx]);
    turns[idx].store(UNUSED, std::memory_order_release);
    return v;
  }


  bool try_pop(T& value) noexcept 
  { 
    auto tail_ = tail.load(std::memory_order_acquire);
    auto idx = index(tail_);
    do {
      if (closed) return false;
      if (turns[idx].load(std::memory_order_acquire) == USED) {
        if (tail.compare_exchange_strong(tail_,tail_+1)) {
          value = std::move(elements[idx]);
          turns[idx].store(UNUSED, std::memory_order_relaxed);
          return true;
        }
      } else {
        auto const ptail_ = tail_;
        auto tail_ = tail.load(std::memory_order_acquire);
        if (ptail_ == tail_)
          return false;
      }
    } while (true);
  }


  void close() noexcept
  { closed = true; }

  bool is_closed() noexcept
  { return closed == true; }


  bool empty() const noexcept
  { return approx_size() <= 0; }


  size_t approx_size() const noexcept
  {
    return head.load(std::memory_order_relaxed) - 
           tail.load(std::memory_order_relaxed);
  }

private:

  constexpr uint64_t index(size_t idx) const noexcept
  { return idx % capacity; }

  // constexpr uint64_t turn(size_t idx) const noexcept
  // { return idx / capacity; }

};
