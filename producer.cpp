#include <thread>
#include <chrono>
#include "producer.h"
#include "monitor.h"
using namespace std;
Producer::Producer(int sleepTime, RequestType type)
{
    this->sleepTime = sleepTime;
    this->prodType = type;
}

void *Producer::produce(void *prodArgs)
{
    Monitor *monitor = ((prodEntityArgs *)prodArgs)->simMonitor;
    Producer *currProducer = ((prodEntityArgs *)prodArgs)->producerObj;
    if (currProducer->sleepTime > 0)
    {
        this_thread::sleep_for(chrono::milliseconds(currProducer->sleepTime));
    }
    this_thread::sleep_for(chrono::milliseconds(currProducer->sleepTime));
    while (true)
    {
        int itemsProduced = monitor->insert(currProducer->prodType);
        if (itemsProduced == 0)
        {
            break;
        }
        if (currProducer->sleepTime > 0)
        {
            this_thread::sleep_for(chrono::milliseconds(currProducer->sleepTime));
        }
    }
    return nullptr;
}
