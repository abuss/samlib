#pragma once

#include <tuple>

namespace samlib {

  using std::get;

  template<size_t I, typename P, typename Val>
  bool send_to(P& ports_, Val&& val)
  {
    return ports_.template send_to<I>(val);
  }
  
  template <typename... T>
  struct ports 
    : std::tuple<T...>
  {
  
    constexpr ports() = default;
  
    constexpr ports(T&&... args)
      : std::tuple<T...>(args...)
    { }
  
    template<size_t i, typename Val>
    constexpr bool send_to(Val&& val)
    {
      return std::get<i>(*this)->send(val);
    }
  
    static constexpr std::size_t size() { return sizeof...(T); }
  };

} // namespace samlib
