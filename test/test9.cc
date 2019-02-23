
#include <unistd.h>
#include <iostream>
#include <tuple>
#include <vector> 

#include <samlib/agent.hpp>
#include <samlib/tasks.hpp>
#include <samlib/environment.hpp>


auto fn_factory(int num)
{
  return [=](size_t val) {
    printf("P%d -> %lu\n",num,val++);
    return val;
  };
}


int main(int argc, char *argv[])
{
 
  typedef samlib::environment<samlib::base_state> state_t;
  typedef samlib::agent<state_t,samlib::empty_state,size_t,size_t> agent_t;

  int n = 5;
  if (argc>1)
    n = atoi(argv[1]);
  
  state_t st;

  agent_t* p = nullptr;
  agent_t* tmp = nullptr;
  agent_t* first = nullptr;
  
  
  for (int i=0;i<n;++i) {
    p = new agent_t(st,samlib::transform(fn_factory(i)));
    p->start();
    if (i==0) {
      first = p;
      tmp = p;
    }
    else {
      tmp->set_outputs(*p);
      tmp = p;
    }
  }

  if (first!=tmp) {
    p->set_outputs(*first);
  }

  sleep(1);

  std::cout << "Starting\n";
  first->send(1);

  sleep(1);
  printf("------------ Time's up ---------------\n");

  st.stop_agents();
}
