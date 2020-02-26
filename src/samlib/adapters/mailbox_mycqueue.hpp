// Mailbox adapter using mycqueue

#pragma once

#include <iostream>
#include <optional>
#include <chrono>
#include <jthread.hpp>

#include "mycqueue.hpp"


namespace samlib
{
 
  template<typename T>
  class mailbox
    : mycqueue<T>
  {

    using base_t = mycqueue<T>;
    bool stop_flag = false;

  public:

    mailbox(size_t capacity_=64)
      : base_t(capacity_)
    { }

    void stop(bool flag)
    {
      stop_flag = flag; 
    }

    size_t size_approx() const
    {
      return base_t::approx_size();
    }
        
    typedef T value_type;

    bool send(const value_type& value)
    {
      if (!stop_flag) {
        this->push(value);
        return true;
      }
      return false;
    }

    bool send(value_type&& value)
    {
      if (!stop_flag) {
        this->push(std::forward<value_type>(value));
        return true;
      }
      return false;
    }

    std::optional<value_type> receive()
    {
      if (!stop_flag) {
        return this->pop();
      }
      return std::nullopt;
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (!stop_flag && this->try_pop(value))
        return std::make_optional(std::move(value));
      return std::nullopt;
    }

    void close() {
      stop_flag = true;
      base_t::close();
    }

  };

}
