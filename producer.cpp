// Name: Carlos Reyes REDID: 131068259
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
    this->sleepTime = sleepTime; // The time to sleep in milliseconds
    this->prodType = type;       // The type produce by this producer
}

void *Producer::produce(void *prodArgs)
{
    /*
    This is a static function for the producer thread to run. It takes in a pointer to a structure of arguments
    with the producer object and the monitor object structure
    This runs a thread to insert to the monitor which manages the buffer for producers and consumer robot threads
    */
    Monitor *monitor = ((prodEntityArgs *)prodArgs)->simMonitor;        // Get monitor from arguments
    Producer *currProducer = ((prodEntityArgs *)prodArgs)->producerObj; // Get current producer from arguments
    // We may need an extra sleep here
    while (true)
    {
        // This loop continuously tries insert requests until the last requests on the last maximum request
        if (currProducer->sleepTime > 0)
        {
            // We sleep if we have a sleep time for the thread if we dont we proceed with our first insert without sleeping
            this_thread::sleep_for(chrono::milliseconds(currProducer->sleepTime));
        }
        // Well let our monitor handle the insert for the current producer and its type of requests it can produce
        // this returns zero requests when the max amount of requests produced has been hit for both sleeping threads and the last thread that produced the max requests
        int requestsProduced = monitor->insert(currProducer->prodType);
        if (requestsProduced == 0)
        {
            // This branch is to break the loop when the max amount of requests has been hit for the sleeping threads as well
            break;
        }
    }
    // return a null pointer after thread is done
    return nullptr;
}
