#pragma once

#include <tuple>

namespace samlib {

  template <typename... T>
  using ports = std::tuple<T...>; 
  
  using std::get;

  template<size_t I, typename P, typename Val>
  constexpr bool send_to(const P& ports_, Val&& val)
  {
    return std::get<I>(ports_)->send(std::forward<Val>(val));
  }

  template <typename... T>
  ports<T...> make_ports(T... args)
  {
    return std::make_tuple(args...);
  }
  
} // namespace samlib
