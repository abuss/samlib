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

  void start()
  {
    _executor = executor_type([&](std::stop_token st) {
      this->run(st);
    });
  }

  void stop()
  {
    _executor.request_stop();
  }

  void wait()
  {
    _executor.join();
  }

  virtual void run(const std::stop_token& st) = 0;

private:
  executor_type _executor;
};

} // namespace samlib
