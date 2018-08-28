#pragma once

#include <chrono>

#include <samlib/ports.hpp>

namespace samlib
{

auto transform = [](auto&& fn)
{
  return [fn](auto& state, auto&, auto& in_port, auto& out_ports) {
    if (auto data = in_port.try_receive()) {
      auto new_data = fn(*data);
      samlib::send_to<0>(out_ports, new_data);
    }
    else {
      std::this_thread::yield();
    }
  };
};


auto generator = [](auto&& fn)
{
  // size_t ngen = 0;

  return [fn](auto& state, auto&, auto& in_port, auto& out_ports) {
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


auto sink = [](auto&& fn)
{
  return [fn](auto& state, auto&, auto& in_port, auto& out_ports) {
    if (auto data = in_port.try_receive())
      fn(*data);
    else
      std::this_thread::yield();
  };
};
    
}; // namespace samlib
