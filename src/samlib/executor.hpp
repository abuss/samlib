#pragma once

#include <thread>
#include <vector>

namespace samlib
{

class executor
{
  std::vector<std::jthread> workers;

public:
  executor() noexcept
  { }

  template<class F, class... Args>
  explicit executor(F&& f, uint num_workers, Args&&... args)
  {
    for (uint i = 0; i < num_workers; ++i) {
      workers.emplace_back(std::forward<F>(f), std::forward<Args>(args)...);
    }
  }

  void join()
  {
    for (auto& wrk : workers)
      if (wrk.joinable())
        wrk.join();
  }

  void request_stop()
  {
    for (auto& wrk : workers)
      wrk.request_stop();
  }

  void detach()
  {
    for (auto& wrk : workers)
      wrk.detach();
  }
};


} // namespace samlib
