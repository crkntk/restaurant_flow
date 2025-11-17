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

void *Consumer::consume(void *consArgs)
{

    Monitor *monitor = ((consEntityArgs *)consArgs)->simMonitor;
    Consumer *currConsumer = ((consEntityArgs *)consArgs)->consumerObj;
    while (true)
    {
        int consumedAmount = monitor->remove(currConsumer->ConsType);
        if (consumedAmount == 0)
        {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(currConsumer->timeSleep));
    }
    return nullptr;
}
