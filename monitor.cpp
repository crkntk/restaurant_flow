#include "monitor.h"

Monitor::Monitor(int genCapacity, int vipCapacity)
{
    this->normalCapacity = genCapacity;
    this->VIPCapacity = vipCapacity;
    this->queueGenReq = 0;
    this->queueVipReq = 0;
    pthread_cond_init(&this->seatsAvail, NULL);
    pthread_cond_init(&this->unconsumedSeats, NULL);
    pthread_cond_init(&this->VipSeatsAvail, NULL);
    pthread_mutex_init(&this->mutex, NULL);
}

void Monitor::insert(RequestType request){
    bool onlyItem;
    pthread_mutex_lock(&mutex);
    while(this->queueGenReq >= this->normalCapacity || (this->queueVipReq >=this->VIPCapacity && request == VIPRoom)){
        pthread_cond_wait(&seatsAvail,&this->mutex);
    }
    onlyItem = (queueGenReq == 0);
    this->buffer.push(request);

    if(onlyItem){
        pthread_cond_signal(&this->unconsumedSeats);
    }
    pthread_mutex_unlock(&this->mutex);
}
RequestType Monitor::remove(){
    RequestType request;
    bool atCapacity;
    pthread_mutex_lock(&mutex);
    while(this-> queueGenReq == 0){
        pthread_cond_wait(&unconsumedSeats, &this->mutex);
    }
    atCapacity = queueGenReq == this->normalCapacity;
    request = this->buffer.front();
    this->buffer.pop();
    if(atCapacity){
        pthread_cond_signal(&this->seatsAvail);
    }
    pthread_mutex_unlock(&this->mutex);
    return request;
}