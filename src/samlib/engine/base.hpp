// #pragma once

// Base engine

namespace samlib
{

template <typename Goal>
struct base_engine
{
  struct shared_state
  {
    bool terminate;
    bool report;
    Goal goal;
  };

  shared_state global_state;

  base_engine()
    : global_state{false, true, Goal()}
  { }
};

} // namespace samlib
