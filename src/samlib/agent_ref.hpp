#pragma once

namespace samlib
{

template<typename A>
class agent_ref
{
  using agent_t = A;
  std::shared_ptr<agent_t> ptr;

public:
  constexpr agent_ref()
    : ptr(nullptr)
  { }

  virtual ~agent_ref() { }

  constexpr explicit agent_ref(std::shared_ptr<agent_t> a)
    : ptr(a)
  { }

  agent_ref(agent_ref&& other) = default;

  agent_ref(const agent_ref& other) = default;

  agent_ref& operator=(const agent_ref& other)
  {
    ptr.reset(other.ptr);
    return *this;
  }

  agent_ref& operator=(agent_ref&& other)
  {
    ptr = std::move(other.ptr);
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

  void stop() const
  {
    ptr->stop();
  }

  agent_t& ref_agent() const
  {
    return *ptr;
  }
};

} // namespace samlib
