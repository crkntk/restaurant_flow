#include <thread>
#include <chrono>
#include "consumer.h"
#include "monitor.h"
#include "seating.h"
using namespace std;
Consumer::Consumer(int timeSleep, ConsumerType consumerType)
{
    this->timeSleep = timeSleep;
    this->ConsType = consumerType;
}

void *Consumer::consume(void *entityArgs)
{

    Monitor *monitor = ((entityConsArgs *)entityArgs)->simMonitor;
    Consumer *currConsumer = ((entityConsArgs *)entityArgs)->consumerObj;
    while (true)
    {
        RequestType *consumedType = monitor->remove(currConsumer->ConsType);
        if (consumedType == nullptr)
        {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(currConsumer->timeSleep));
    }
}
