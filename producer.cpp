#include "producer.h"

Producer::Producer(int sleepTime, RequestType type){
this->sleepTime = sleepTime;
this->prodType = type;
}

void*Producer::produce(void *args){

    while(true){

        int itemsProduced = 
    }
}

