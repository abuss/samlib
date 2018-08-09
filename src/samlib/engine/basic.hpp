#pragma once

#include <iostream>
#include <cassert>
#include <unistd.h>

#include <thread>

// #include <spga/chromo_pack.hpp>

#include <spga/tasks/generator.hpp>
#include <spga/tasks/fitness.hpp>
#include <spga/tasks/selector.hpp>
#include <spga/tasks/mutator.hpp>
#include <spga/tasks/cross_over.hpp>
#include <spga/tasks/report.hpp>

// #include <concurrentqueue.h>

// #define NO_CONCURRENT_QUEUE
#include <spga/mailbox.hpp>

namespace spga
{

  template<typename Chrm, typename Oper>
  struct basic_engine
  {
    struct share_state
    {
      bool terminate;
      bool report;
      int  delay;
    };

    typedef Chrm                                  chromosome_type;
    typedef Oper                                  operations_t;
    typedef std::function<void(chromosome_type)>  report_type;
    typedef spga::mailbox<chromosome_type>        queue_type;

    Oper  oper;
    std::function<void(chromosome_type)> report;
    share_state state;

    basic_engine(Oper op, report_type rep, int delay=200000)
      : oper{op},
        report{rep},
        state {false, true, delay}
    { }

    void run(size_t n, size_t pop_size, size_t chromo_size, size_t n_mutation,
             float  upper_limit, double goal)
    {
      queue_type fitq;
      queue_type selq;
      queue_type mutq;
      queue_type xoverq;
      queue_type repq;

      spga::report_task<queue_type, share_state, chromosome_type>
        reptask(repq, state);
      std::thread rep([&]{ reptask.run(report); });

      spga::generator_task<queue_type>
        gentask(state.terminate, fitq);
      std::thread gen([&]{ gentask.run(pop_size, 
        std::bind(&Oper::generate, oper)); });

      spga::fitness_task<queue_type, chromosome_type, operations_t>
        fittask(state.terminate, fitq, selq, oper);
      std::thread fit([&]{ fittask.run(); });

      spga::selector_task<queue_type, chromosome_type>
        seltask(state.terminate, selq, xoverq, mutq, fitq,
                state.report, repq, goal, upper_limit);
      std::thread sel([&]{ seltask.run(); });

      spga::mutator_task<queue_type, chromosome_type, operations_t>
        muttask(state.terminate, mutq, fitq, oper);
      std::thread mut([&]{ muttask.run(); });

      spga::cross_over_task<queue_type, chromosome_type, operations_t>
        xovertask(state.terminate, xoverq, mutq, fitq, oper);
      std::thread xover([&]{ xovertask.run(); });

      sleep(n);
      state.terminate = true;
      gen.join();
      fit.join();
      mut.join();
      xover.join();
      sel.join();
      rep.join();
    }

  };

}
