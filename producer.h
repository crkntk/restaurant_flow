#include <atomic>
#include "seating.h"
#include "monitor.h"
using namespace std;
#ifndef PRODUCER_H
#define PRODUCER_H
/*
This class is the producer class it keeps track of the type of requests this producer makes and
the sleeping time for this thread. The static void function is for the posix thread creation and initialization
*/
class Producer
{
public:
    Producer(int sleepTime, RequestType type); // Constructor for starting our producer with sleep time and the type of request produced
    RequestType prodType;                      // The type of request produced by this producer
    int sleepTime;                             // The time this producer needs to sleep for the thread in milliseconds
    static void *produce(void *args);          // The function to handle the producer producing and object being handled by the monitor. It is static to use with posix thread creation
    /*
    This structure is for the posix thread creation in our static produce functions
    We need this to pass the arguments as a pointer to this structure to parse through them
    This has the needed arguments for the produce thread:
    The pointer to the monitor to manage the consumer producer problem and
    the producer object to get the the attributes needed for the thread management like sleep
    */
    struct prodEntityArgs
    {
        Monitor *simMonitor;   // Pointer to the simulation monitor for handling the insertion and production of requests
        Producer *producerObj; // The producer object used for the thread function
    };
};

#endif