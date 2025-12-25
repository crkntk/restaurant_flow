DINESEATING — Restaurant Flow Control (Producer/Consumer Simulation)

Overview

      This project simulates a restaurant seating management system using the classic bounded-buffer producer/consumer pattern.
      
      Two producer threads (“greeter robots”) generate seating requests:
      
      General table requests
      
      VIP room requests
      
      Two consumer threads (“concierge robots”) process requests in FIFO order:
      
      T-X
      
      Rev-9
      
      A shared request queue acts as a bounded buffer:
      
      Total queue capacity: 20 requests
      
      VIP requests in-queue limit: 6 requests
      
      This was built as an Operating Systems synchronization assignment using a monitor (mutex + condition variables) and a semaphore barrier for clean shutdown coordination. 
      
      Assignment 04 no tricks
      
      Key Concepts / What This Demonstrates
      
      POSIX threads (pthreads) with shared-state synchronization
      
      Monitor-style concurrency control:
      
      pthread_mutex_t for mutual exclusion
      
      pthread_cond_t for coordination (queue full/empty + VIP capacity constraint)
      
      FIFO correctness under nondeterministic scheduling
      
      Precedence constraint / shutdown signaling using an unnamed semaphore (barrier)
      
      CLI parameter parsing with getopt
      
      Debug-friendly build with -g3 for gdb

Build

      Requirements: Linux (tested in Ubuntu-style environments), g++
      
      Compile:
      make
      
      Clean:
      make clean
      
      Output binary:
      ./dineseating

Run

      Basic:
      ./dineseating
      
      With options:
      ./dineseating -s N -x N -r N -g N -v N
      
      Options (all optional):
      
      -s N Total number of seating requests to produce (default: 110)
      
      -x N Average consume time (ms) for T-X consumer thread
      
      -r N Average consume time (ms) for Rev-9 consumer thread
      
      -g N Average produce time (ms) for General-table producer thread
      
      -v N Average produce time (ms) for VIP producer thread

Notes:

      “Sleep” delays are intentionally performed OUTSIDE the critical section so other threads can proceed while one thread simulates work.
      
      How It Works (Implementation Notes)
      
      Core module: Monitor (monitor.h / monitor.cpp)
      
      The Monitor owns:
      
      A FIFO queue<RequestType> buffer
      
      Counters for:
      
      total produced vs. production limit
      
      current queue occupancy
      
      VIP occupancy within the queue
      
      production/consumption statistics by type and by consumer
      
      Synchronization primitives:
      
      mutex: protects all queue and counter state
      
      condition variables:
      
      seatsAvail (signals space available in overall queue)
      
      VipSeatsAvail (signals space available for VIP requests)
      
      unconsumedSeats (signals queue is non-empty for consumers)
      
      barrier semaphore (signals main thread when the last request is consumed)
      
      Insert path (Producer -> Monitor::insert):
      
      Locks the mutex
      
      If production limit reached:
      
      wakes any blocked threads (to avoid deadlock)
      
      returns 0 (producer exits loop)
      
      Otherwise waits while:
      
      queue is full (capacity 20), OR
      
      request is VIP and VIP limit is reached (capacity 6)
      
      Pushes request into FIFO queue
      
      Updates stats and logs the mutation while still in the critical section
      
      Signals consumers that an item is available
      
      Remove path (Consumer -> Monitor::remove):
      
      Locks the mutex
      
      Waits while queue is empty, unless production is finished
      
      Pops from FIFO queue
      
      Updates stats and logs the mutation while still in the critical section
      
      Signals producers that space is available:
      
      VIP producer signaled when a VIP slot is freed
      
      otherwise signals general space condition
      
      If the last request has been consumed AND production is complete:
      
      prints final production/consumption history
      
      posts the barrier semaphore to allow main() to exit

Shutdown strategy:

      main() joins producers (ensures production completes cleanly)
      
      main() then waits on the barrier semaphore
      
      when the final consumer consumes the last request, it signals the semaphore, allowing main to exit (and the OS ends remaining blocked threads)
      
      Project Structure
      
      main.cpp CLI parsing, thread creation, joining producers, barrier wait
      
      monitor.* Shared queue + synchronization + statistics + logging calls
      
      producer.* Producer object + pthread entry function
      
      consumer.* Consumer object + pthread entry function
      
      log.* / seating.h Provided logging/types used to match expected output format
      
      run_tests.sh Convenience script to run multiple scenarios
      
      Makefile Builds dineseating with pthread + debug symbols

Testing

      You can run the provided script:
      ./run_tests.sh

      Or try:
      ./dineseating -s 100 -x 20 -r 35 -g 15 -v 10
