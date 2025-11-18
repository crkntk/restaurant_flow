#include <thread>
#include <chrono>
#include "producer.h"
#include "monitor.h"
using namespace std;
Producer::Producer(int sleepTime, RequestType type)
{
    /*
    This is the constructor for the producer the amount of time to sleep this producer and the
    request types produced by this producer
    */
    this->sleepTime = sleepTime; //The time to sleep in milliseconds
    this->prodType = type; //The type produce by this producer
}

void *Producer::produce(void *prodArgs)
{
    /*
    This is a static funciton for the producer thread to run. It takes in a pointer to a sturcture of arguments
    
    */
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
