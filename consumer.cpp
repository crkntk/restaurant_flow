#include "consumer.h"
#include "monitor.h"
#include "seating.h"
Consumer::Consumer(int timeSleep, ConsumerType consumerType){
    this->timeSleep = timeSleep;
    this->ConsType = consumerType;
}

void* Consumer::consume(void *entityArgs){

    Monitor *monitor = ((entityFlowArgs*)entityArgs)->simMonitor;
    Consumer *currConsumer = ((entityFlowArgs*)entityArgs)->consumerObj;
    while(true){
        RequestType* consumedType = monitor->remove();
        if(consumedType == nullptr){
            break;
        }
    }
}

