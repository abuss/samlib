#pragma once

namespace samlib
{

template <typename Derived>
struct environment
{

  bool terminate = false;

  template <typename A, typename... Args>
  A make_agent(Args... args)
  {
    return A(*static_cast<Derived *>(this), args...);
  }

  template <typename A, typename... Args>
  A *make_agent_ptr(Args... args)
  {
    return new A(*static_cast<Derived *>(this), args...);
  }
};

} // namespace samlib
