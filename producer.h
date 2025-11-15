#include <atomic>
#include "seating.h"

using namespace std;
#ifndef PRODUCER_H
#define PRODUCER_H
struct entityFlowArgs {
    Monitor* simMonitor;
    Producer* producerObj;
};
class Producer
{
public:
    Producer(int sleepTime, RequestType type);
    RequestType prodType;
    int sleepTime;
    static void *produce(void *args);
};
#endif