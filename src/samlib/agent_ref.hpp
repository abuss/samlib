#pragma once

#include "agent.hpp"

namespace samlib
{

  template<typename A>
  class agent_ref
  {
    using agent_t = A;
    agent_t* ptr;

  public:

    constexpr agent_ref()
      : ptr(nullptr)
    { }

    constexpr explicit agent_ref(agent_t* a)
      : ptr(a)
    { }

    agent_ref(const agent_ref& other)
      : ptr(other.ptr)
    { }

    agent_ref& operator=(const agent_ref& other)
    {
      ptr = other.ptr;
      return *this;
    }

    template<typename T>
    constexpr bool send(const T& value) const
    {
      return ptr->send(value);
    }

    template<typename T>
    constexpr bool send(T&& value) const
    {
      return ptr->send(value);
    }

    void start() const
    {
      ptr->start();
    }

    void wait() const
    {
      ptr->wait();
    }

  };

}