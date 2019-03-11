#pragma once

#include <chrono>

#include <samlib/ports.hpp>

namespace samlib
{

using namespace std::literals::chrono_literals;

template<typename Fn, typename O>
auto transform(Fn fn, O& out)
{
  return [fn,&out](auto& state, auto, auto& in_port) {
            if (auto data = in_port.try_receive()) {
              out.send(fn(std::move(*data)));
            }
            else {
              std::this_thread::yield();
            }
          };
};


template<typename Fn, typename O>
auto generator(Fn fn, O& out)
{
  // size_t ngen = 0;

  return [fn,&out](auto& state, auto, auto& in_port) {
          if (auto dat = in_port.try_receive()) {
            auto n = *dat;
            while ((n > 0) && (!state.terminate)) {
              out.send(fn(n));
              // global.ngen += 1;
              // ++ngen;
              --n;
            }
          }
          else {
            std::this_thread::sleep_for(20ms);
          }
        };
};


auto sink = [](auto fn)
{
  return [fn](auto& state, auto, auto& in_port) {
    if (auto data = in_port.try_receive())
      fn(*data);
    else
      std::this_thread::yield();
  };
};
    
}; // namespace samlib
