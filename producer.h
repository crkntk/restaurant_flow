#include <atomic>
#include "seating.h"
#include "monitor.h"
using namespace std;
#ifndef PRODUCER_H
#define PRODUCER_H
class Producer
{
public:
    Producer(int sleepTime, RequestType type);
    RequestType prodType;
    int sleepTime;
    static void *produce(void *args);
};
struct entityProdArgs
{
    Monitor *simMonitor;
    Producer *producerObj;
};
#endif