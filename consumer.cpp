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
    This function is for the posix library to run the current conumer thread. It arguments is a structure to a

    */
    Monitor *monitor = ((consEntityArgs *)consArgs)->simMonitor;
    Consumer *currConsumer = ((consEntityArgs *)consArgs)->consumerObj;
    while (true)
    {
        if (currConsumer->timeSleep > 0)
        {
            this_thread::sleep_for(chrono::milliseconds(currConsumer->timeSleep));
        }
        int consumedAmount = monitor->remove(currConsumer->ConsType);
    }
    return nullptr;
}
