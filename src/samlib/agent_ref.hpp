#pragma once

namespace samlib
{

template<typename A>
class agent_ref
{
  using agent_t = A;
  std::shared_ptr<agent_t> ptr;
  using message_t = typename agent_t::message_type;

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

  constexpr bool send(const message_t& value) const
  {
    return ptr->send(value);
  }

  constexpr bool send(message_t&& value) const
  {
    return ptr->send(std::forward<message_t>(value));
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
