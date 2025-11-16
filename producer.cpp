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

void *Producer::produce(void *entityStruct)
{
    Monitor *monitor = ((entityProdArgs *)entityStruct)->simMonitor;
    Producer *currProducer = ((entityProdArgs *)entityStruct)->producerObj;
    while (true)
    {
        int itemsProduced = monitor->insert(currProducer->prodType);
        if (itemsProduced == 0)
        {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(currProducer->sleepTime));
    }
    return nullptr;
}
