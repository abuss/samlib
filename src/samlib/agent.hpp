#pragma once

#include <samlib/agent/base.hpp>
#include <samlib/ports.hpp>

namespace samlib
{

template <typename GS, typename Tin, typename... Tout>
class agent
  : public base<Tin>
{
protected:

  typedef ports<mailbox<Tout>*...> ports_t;
  
  GS*     global_state;
  ports_t outputs;

public:

  agent()
    : global_state{nullptr}
  { }

  agent(GS &gstate)
    : global_state(&gstate)
  { }
  
  template <int i, typename P>
  agent& set_output(P &p)
  {
    samlib::get<i>(outputs) = &p.mbox();
    return *this;
  }

  template <typename... Ps>
  agent& set_outputs(Ps &... ps)
  {
    outputs = std::make_tuple(&ps.mbox()...);
    return *this;
  }

};

} // namespace samlib
