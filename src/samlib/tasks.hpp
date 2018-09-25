#pragma once

#include <chrono>

#include <samlib/ports.hpp>

namespace samlib
{

template<typename Fn>
auto transform(Fn fn)
{
  return [fn](auto& state, auto, auto& in_port, const auto& out_ports) {
            if (auto data = in_port.try_receive()) {
              samlib::send_to<0>(out_ports, fn(std::move(*data)));
            }
            else {
              std::this_thread::yield();
            }
          };
};


auto generator2 = [](auto fn)
{
  // size_t ngen = 0;

  return [fn](auto& state, auto, auto& in_port, const auto& out_ports) {
    if (auto dat = in_port.try_receive()) {
      auto n = *dat;
      auto ngen = n;
      while ((n > 0) && (!state.terminate)) {
        samlib::send_to<0>(out_ports,fn(n));
        // global.ngen += 1;
        // ++ngen;
        --n;
      }
      printf("generated: %ul\n",ngen);
    }
    else {
      std::this_thread::yield();
    }
  };
};

template<typename Fn>
auto generator(Fn fn)
{
  // size_t ngen = 0;

  return [fn](auto& state, auto, auto& in_port, const auto& out_ports) {
          if (auto dat = in_port.try_receive()) {
            auto n = *dat;
            while ((n > 0) && (!state.terminate)) {
              samlib::send_to<0>(out_ports,fn(n));
              // global.ngen += 1;
              // ++ngen;
              --n;
            }
          }
          else {
            std::this_thread::yield();
          }
        };
};


auto sink = [](auto fn)
{
  return [fn](auto& state, auto, auto& in_port, const auto& out_ports) {
    if (auto data = in_port.try_receive())
      fn(*data);
    else
      std::this_thread::yield();
  };
};
    
}; // namespace samlib
