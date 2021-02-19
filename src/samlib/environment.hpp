#pragma once
/**
    \file environment.hpp
    Defines a global state that is used to dedfine an environment where the
    agents can share data
*/

#include <samlib/runtime.hpp>

namespace samlib
{
struct environment
  : public runtime<environment>
{ };

} // namespace samlib
