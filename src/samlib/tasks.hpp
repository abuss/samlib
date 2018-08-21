#pragma once

#include <chrono>

#include <samlib/ports.hpp>

namespace samlib
{

auto transform = [](auto&& fn)
{
  using namespace std::literals::chrono_literals; 

  return [fn](auto& state, auto&, auto& in_port, auto& out_ports) {
    if (auto data = in_port.try_receive()) {
      auto new_data = fn(*data);
      samlib::send_to<0>(out_ports, new_data);
    }
    else {
      // TODO: check if there is a better way to do this
      std::this_thread::sleep_for(1us);
    }
  };
};


auto generator = [](auto&& fn)
{
  // size_t ngen = 0;
  using namespace std::literals::chrono_literals; 

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
      // TODO: check if there is a better way to do this
      std::this_thread::sleep_for(1us);
    }
  };
};


auto sink = [](auto&& fn)
{
  using namespace std::literals::chrono_literals; 

  return [fn](auto& state, auto&, auto& in_port, auto& out_ports) {
    if (auto data = in_port.try_receive())
      fn(*data);
    else
      std::this_thread::sleep_for(1us);
  };
};
    
}; // namespace samlib
