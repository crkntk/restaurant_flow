#include <thread>
#include <chrono>
#include "consumer.h"
#include "monitor.h"
#include "seating.h"
using namespace std;
Consumer::Consumer(int timeSleep, ConsumerType consumerType)
{
    /*
    This is a constructor for the current consumer that hold the sleeping amount for the consumer thread and the type/name of the consumer
    */
    this->timeSleep = timeSleep;
    this->ConsType = consumerType;
}

void *Consumer::consume(void *consArgs)
{
    /*
    This function is for the posix library to run the current conumer thread. It argument is a pointer to a structure
    that hols the simulation monitor and the consumer object with sleep and type attributes
    */
    Monitor *monitor = ((consEntityArgs *)consArgs)->simMonitor;        // extract monitor from arg structure
    Consumer *currConsumer = ((consEntityArgs *)consArgs)->consumerObj; // extract consumer object from argument ponter
    while (true)
    {
        // This while loop tries to insert a request type using the monitor which manages the removing and request queue
        // The monitor has a barrier semaphore that signals when last has been removed to main that will
        // eventually make the main thread proceed to finish and kill all threads that were waiting or running this loop
        if (currConsumer->timeSleep > 0)
        {
            // This branch checks if the current consumer thread has a sleep above zero if not it should not call the function and waste cpu time
            this_thread::sleep_for(chrono::milliseconds(currConsumer->timeSleep));
        }
        // Call to remove
        int consumedAmount = monitor->remove(currConsumer->ConsType);
    }
    // This returns a nulll pointer for the void return fo the function
    return nullptr;
}
