#include "monitor.h"

Monitor::Monitor(int genCapacity, int vipCapacity = MONITOR_GEN_CAP, int maxProdReq = MONITOR_VIP_CAP)
{
    this->normalCapacity = genCapacity;
    this->VIPCapacity = vipCapacity;
    this->maxProdRequests = maxProdReq;
    this->queueGenReq = 0;
    this->queueVipReq = 0;
    this->reqProduced = 0;
    this->maxReqHit = false;
    pthread_cond_init(&this->seatsAvail, NULL);
    pthread_cond_init(&this->unconsumedSeats, NULL);
    // pthread_cond_init(&this->VipSeatsAvail, NULL);
    pthread_mutex_init(&this->mutex, NULL);
}

int Monitor::insert(RequestType request)
{
    bool onlyItem;
    pthread_mutex_lock(&mutex);
    if (this->maxProdRequests <= this->reqProduced)
    {
        this->maxReqHit = true;
        pthread_cond_broadcast(&unconsumedSeats);
        pthread_cond_broadcast(&seatsAvail);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    while ((this->queueVipReq >= this->VIPCapacity && request == VIPRoom) || this->queueGenReq >= this->normalCapacity)
    {
        if (this->maxProdRequests <= this->reqProduced)
        {
            this->maxReqHit = true;
            pthread_cond_broadcast(&unconsumedSeats);
            pthread_cond_broadcast(&seatsAvail);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        pthread_cond_wait(&seatsAvail, &this->mutex);
    }

    onlyItem = (queueGenReq == 0);
    this->buffer.push(request);
    this->reqProduced += 1;
    this->queueGenReq += 1;
    if (request == VIPRoom)
    {
        this->queueVipReq += 1;
    }

    if (onlyItem)
    {
        pthread_cond_signal(&this->unconsumedSeats);
    }
    if (this->maxProdRequests <= this->reqProduced)
    {
        this->maxReqHit = true;
        pthread_cond_broadcast(&unconsumedSeats);
        pthread_cond_broadcast(&seatsAvail);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&this->mutex);
    return 1;
}
RequestType Monitor::remove()
{
    RequestType request;
    bool atCapacity;
    pthread_mutex_lock(&mutex);
    while (this->queueGenReq == 0)
    {
        pthread_cond_wait(&unconsumedSeats, &this->mutex);
    }
    atCapacity = (queueGenReq == this->normalCapacity);
    request = this->buffer.front();
    this->buffer.pop();
    if (atCapacity)
    {
        pthread_cond_signal(&this->seatsAvail);
    }
    pthread_mutex_unlock(&this->mutex);
    return request;
}