#pragma once

#include <chrono>

namespace samlib
{

using namespace std::literals::chrono_literals;

// TODO: Define version to pass the state to the functions

template<typename Fn, typename O>
auto transform(Fn fn, O& out)
{
  return [fn, &out](auto& agent) {
    if (auto data = agent.try_receive()) {
      out.send(fn(std::move(*data)));
    }
  };
}

template<typename Fn, typename O>
auto transform(Fn fn, O&& out)
{
  return [fn, out](auto& agent) {
    if (auto data = agent.try_receive()) {
      out.send(fn(std::move(*data)));
    }
  };
}


template<typename Fn, typename O>
auto generator(Fn fn, O& out)
{
  return [fn, &out](auto& agent) {
    if (auto dat = agent.try_receive()) {
      auto n = *dat;
      while ((n > 0) && agent.environment.active()) {
        out.send(fn(n));
        --n;
      }
    }
  };
}

template<typename Fn, typename O>
auto generator(Fn fn, O&& out)
{
  return [fn, out](auto& agent) {
    if (auto dat = agent.try_receive()) {
      auto n = *dat;
      while ((n > 0) && agent.environment.active()) {
        out.send(fn(n));
        --n;
      }
    }
  };
}


auto sink = [](auto fn) {
  return [fn](auto& agent) {
    if (auto data = agent.try_receive())
      fn(*data);
  };
};


template<typename... O>
auto splitter(O&... outs)
{
  return [&](auto& agent) {
    if (auto data = agent.try_receive()) {
      for (auto out : {outs...})
        out.send(*data);
    }
  };
}

template<typename... O>
auto splitter(O&&... outs)
{
  return [=](auto& agent) {
    if (auto data = agent.try_receive()) {
      for (auto out : {outs...})
        out.send(*data);
    }
  };
}


namespace stateless_off
{

  template<typename Fn, typename O>
  auto transform(Fn fn, O& out)
  {
    return [fn, &out](auto& in_port) {
      if (auto data = in_port.try_receive()) {
        out.send(fn(std::move(*data)));
      }
    };
  }

  template<typename Fn, typename O>
  auto transform(Fn fn, O&& out)
  {
    return [fn, out](auto& in_port) {
      if (auto data = in_port.try_receive()) {
        out.send(fn(std::move(*data)));
      }
    };
  }


  template<typename Fn, typename O>
  auto generator(Fn fn, O& out)
  {
    return [fn, &out](auto& in_port) {
      if (auto dat = in_port.try_receive()) {
        auto n = *dat;
        while ((n > 0)) {
          out.send(fn(n));
          --n;
        }
      }
    };
  }

  template<typename Fn, typename O>
  auto generator(Fn fn, O&& out)
  {
    return [fn, out](auto& in_port) {
      if (auto dat = in_port.try_receive()) {
        auto n = *dat;
        while ((n > 0)) {
          out.send(fn(n));
          --n;
        }
      }
    };
  }


  auto sink = [](auto fn) {
    return [fn](auto& in_port) {
      if (auto data = in_port.try_receive())
        fn(*data);
    };
  };


  template<typename... O>
  auto splitter(O&... outs)
  {
    return [&](auto& in_port) {
      if (auto data = in_port.try_receive()) {
        for (auto out : {outs...})
          out.send(*data);
      }
    };
  }


  template<typename... O>
  auto splitter(O&&... outs)
  {
    return [=](auto& in_port) {
      if (auto data = in_port.try_receive()) {
        for (auto out : {outs...})
          out.send(*data);
      }
    };
  }

} // namespace stateless

} // namespace samlib
