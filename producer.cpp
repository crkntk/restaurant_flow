#include "producer.h"
#include "monitor.h"
Producer::Producer(int sleepTime, RequestType type){
this->sleepTime = sleepTime;
this->prodType = type;
}

void*Producer::produce(void *entityStruct){
    Monitor *monitor = ((entityFlowArgs*)entityStruct)->simMonitor;
    Producer *currProducer = ((entityFlowArgs*)entityStruct)->producerObj;
    while(true){
        int itemsProduced = monitor->insert(currProducer->prodType);
        if(itemsProduced==0){
            break;
        }
    }
}

