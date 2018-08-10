#pragma once

#include <samlib/agent/base.hpp>
#include <samlib/ports.hpp>

namespace samlib
{

template <typename GS, typename Tin, typename... Tout>
struct agent
    : public samlib::agent_base<Tin>
{
  typedef ports<samlib::mailbox<Tout> *...> ports_t;

  GS *global_state;
  ports_t outputs;

  agent()
      : global_state{nullptr}
  { }

  agent(GS &gstate)
      : global_state(&gstate)
  { }

  template <int i, typename P>
  agent& set_output(P &p)
  {
    outputs.template get<i>() = &p.mbox();
    return *this;
  }

  template <typename... Ps>
  agent& set_outputs(Ps &... ps)
  {
    outputs = ports(&ps.mbox()...);
    return *this;
  }

};

} // namespace samlib
