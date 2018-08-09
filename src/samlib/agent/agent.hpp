#pragma once

#include <samlib/agent/base.hpp>

namespace samlib
{

template <typename GS, typename Tin, typename... Tout>
struct agent
    : public samlib::agent_base<Tin>
{
  typedef std::tuple<spga::mailbox<Tout> *...> ports_t;

  GS *global_state;
  ports_t outputs;

  agent()
      : global_state{nullptr}
  { }

  agent(GS &gstate)
      : global_state(&gstate)
  { }

  template <int i, typename P>
  agent &set_output(P &p)
  {
    std::get<i>(outputs) = &p;
    return *this;
  }

  template <typename... Ps>
  agent &set_outputs(Ps &... ps)
  {
    outputs = std::make_tuple(&ps...);
    return *this;
  }

  template <int i>
  std::tuple_element<i, ports_t> &get_output()
  {
    return std::get<i>(outputs);
  }

};

} // namespace samlib
