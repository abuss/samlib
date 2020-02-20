#pragma once

#include <chrono>

namespace samlib
{

  using namespace std::literals::chrono_literals;

  template<typename Fn, typename O>
  auto transform(Fn fn, const O& out)
  {
    return [fn,&out](auto&, auto& in_port) {
        if (auto data = in_port.try_receive()) {
          out.send(fn(std::move(*data)));
        }
    };
  }


  template<typename Fn, typename O>
  auto generator(Fn fn, const O& out)
  {
    return [fn,&out](auto&, auto& in_port) {
        if (auto dat = in_port.try_receive()) {
          auto n = *dat;
          while ((n > 0)) {
            out.send(fn(n));
            --n;
          }
        }
    };
  }


  auto sink = [](auto fn)
  {
      return [fn](auto&, auto& in_port) {
          if (auto data = in_port.try_receive())
            fn(*data);
      };
  };


  template<typename... O>
  auto splitter(const O&... outs)
  {
    return [&](auto, auto& in_port) {
        if (auto data = in_port.try_receive()) {
          for (auto out : {outs...})
            out.send(*data);
        }
    };
  }

namespace stateless
{

  template<typename Fn, typename O>
  auto transform(Fn fn, const O& out)
  {
    return [fn,&out](auto& in_port) {
        if (auto data = in_port.try_receive()) {
          out.send(fn(std::move(*data)));
        }
    };
  }


  template<typename Fn, typename O>
  auto generator(Fn fn, const O& out)
  {
    return [fn,&out](auto& in_port) {
        if (auto dat = in_port.try_receive()) {
          auto n = *dat;
          while ((n > 0)) {
            out.send(fn(n));
            --n;
          }
        }
    };
  }


  auto sink = [](auto fn)
  {
      return [fn](auto& in_port) {
          if (auto data = in_port.try_receive())
            fn(*data);
      };
  };


  template<typename... O>
  auto splitter(const O&... outs)
  {
    return [&](auto& in_port) {
        if (auto data = in_port.try_receive()) {
          for (auto out : {outs...})
            out.send(*data);
        }
    };
  }

} // namespace stateless

} // namespace samlib
