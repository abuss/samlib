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
    std::stop_token stop_flag;

  public:

    mailbox(size_t capacity_=64)
      : base_t(capacity_)
    { }

    void set_stop_token(std::stop_token st)
    {
      stop_flag = st; 
    }

    size_t size_approx() const
    {
      return base_t::approx_size();
    }
        
    typedef T value_type;

    bool send(const value_type& value)
    {
      if (!stop_flag.stop_requested()) {
        this->push(value);
        return true;
      }
      return false;
    }

    bool send(value_type&& value)
    {
      if (!stop_flag.stop_requested()) {
        this->push(std::forward<value_type>(value));
        return true;
      }
      return false;
    }

    std::optional<value_type> receive()
    {
      if (!stop_flag.stop_requested()) {
        return this->pop();
      }
      return std::nullopt;
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (this->try_pop(value) && !is_closed() && !stop_flag.stop_requested())
        return std::make_optional(std::move(value));
      return std::nullopt;
    }

    using base_t::close;
    using base_t::is_closed;

  };

}
