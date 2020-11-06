#pragma once

#include <samlib/executor.hpp>

namespace samlib
{

class agent_worker
{

public:
  using executor_type = executor;

  virtual ~agent_worker()
  {
    stop();
  }

  void start(uint num_workers = 1)
  {
    _executor = executor([&](std::stop_token st) {
      this->run(st);
    },
    num_workers);
  }

  void stop()
  {
    _executor.request_stop();
  }

  void wait()
  {
    _executor.join();
  }

  executor_type* get_executor()
  {
    return &_executor;
  }

  virtual void run(const std::stop_token& st) = 0;

private:
  executor_type _executor;
};

} // namespace samlib
