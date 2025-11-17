
#include "seating.h"
#include "monitor.h"
#ifndef CONSUMER_H
#define CONSUMER_H
class Consumer
{
public:
    Consumer(int timeSleep, ConsumerType consumerType);
    int timeSleep;
    ConsumerType ConsType;
    static void *consume(void *args);
};
struct consEntityArgs
{
    Monitor *simMonitor;
    Consumer *consumerObj;
};
#endif