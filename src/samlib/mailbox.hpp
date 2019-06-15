// Mailbox implementation using queues
#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#include "mycqueue.hpp"

  
namespace samlib
{
 
  template<typename T>
  class mailbox
    : mycqueue<T>
  {

    using base_t = mycqueue<T>;

  public:

    mailbox()
      : base_t(8)
    { }

    size_t size_approx() const
    {
      return base_t::approx_size();
    }
        
    typedef T value_type;

    bool send(const value_type& value)
    {
      this->push(value);
      return true;
    }

    bool send(value_type&& value)
    {
      this->push(std::forward<value_type>(value));
      return true;
    }

    value_type receive()
    {
      return this->pop();
      // value_type value;
      // this->pop(value);
      // return value;
    }
    
    std::optional<value_type> try_receive()
    {
      value_type value;
      if (this->try_pop(value) && !is_closed())
        return std::make_optional(std::move(value));
      return std::nullopt;
      // value = this->pop();
      // return std::make_optional(std::move(value));
    }

    using base_t::close;
    using base_t::is_closed;

  };

}
