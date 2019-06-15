#pragma once

#include <chrono>

namespace samlib
{

  using namespace std::literals::chrono_literals;

  template<typename Fn, typename O>
  auto transform(Fn fn, const O& out)
  {
    return [fn,&out](auto&, auto& in_port) {
              out.send(fn(in_port.receive()));
            };
  }


  template<typename Fn, typename O>
  auto generator(Fn fn, const O& out)
  {
    return [fn,&out](auto& state, auto& in_port) {
              auto n = in_port.receive();
              while ((n > 0) && (!state.terminate)) {
                out.send(fn(n));
                --n;
              }
            };
  }


  auto sink = [](auto fn) {
    return [fn](auto&, auto& in_port) {
              fn(in_port.receive());
            };
          };


  template<typename... O>
  auto splitter(const O&... outs) {
    return [&](auto, auto& in_port) {
              auto data = in_port.receive();
              for (auto out : {outs...})
                out.send(data);
          };
  }

} // namespace samlib
