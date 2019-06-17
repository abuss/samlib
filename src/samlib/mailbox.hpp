// Mailbox implementation using queues
#pragma once

#include <iostream>
#include <optional>
#include <chrono>

#ifdef QUEUE_IMPLEMENTATION_concurrentqueue
#include "mailbox_concurrentqueue.hpp"
#endif

#ifdef QUEUE_IMPLEMENTATION_atomic_queue
#include "mailbox_atomic_queue.hpp"
#endif

#ifdef QUEUE_IMPLEMENTATION_MPMCQueue
#include "mailbox_MPMCQueue.hpp"
#endif

#ifdef QUEUE_IMPLEMENTATION_mycqueue
#include "mailbox_mycqueue.hpp"
#endif
