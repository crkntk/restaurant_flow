
#include "seating.h"

#ifndef CONSUMER_H
#define CONSUMER_H
struct entityFlowArgs {
    Monitor* simMonitor;
    Consumer* consumerObj;
};
class Consumer{
public:
    Consumer(int timeSleep,ConsumerType consumerType);
    int timeSleep;
    ConsumerType ConsType;
    static void* consume(void* args);
};
#endif