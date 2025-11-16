#include "monitor.h"
#include "log.h"
Monitor::Monitor(int maxProdReq, sem_t *barrierSem, int genCapacity, int vipCapacity)
{
    this->normalCapacity = genCapacity;
    this->VIPCapacity = vipCapacity;
    this->maxProdRequests = maxProdReq;
    this->queueGenReq = 0;
    this->queueVipReq = 0;
    this->reqProduced = 0;
    this->barrierSem = barrierSem;
    this->maxReqHit = false;
    this->unlockedBarrier = false;
    pthread_cond_init(&this->seatsAvail, NULL);
    pthread_cond_init(&this->unconsumedSeats, NULL);
    pthread_cond_init(&this->VipSeatsAvail, NULL);
    pthread_mutex_init(&this->mutex, NULL);
    for (int i = 0; i < RequestTypeN; i++)
    {
        this->prodByType[i] = 0;
        this->consByType[i] = 0;
        this->queueTypes[i] = 0;
    }
    for (int j = 0; j < ConsumerTypeN; j++)
    {
        this->consByRob[j] = 0;
        this->consByRobType[j] = new unsigned int[RequestTypeN]();
    }
}

int Monitor::insert(RequestType request)
{
    bool onlyItem;
    pthread_mutex_lock(&mutex);
    if (this->maxProdRequests <= this->reqProduced)
    {
        this->maxReqHit = true;
        pthread_cond_broadcast(&unconsumedSeats);
        pthread_cond_broadcast(&VipSeatsAvail);
        pthread_cond_broadcast(&seatsAvail);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    while ((request == VIPRoom && this->queueVipReq >= this->VIPCapacity) || this->queueGenReq >= this->normalCapacity)
    {
        if (this->maxProdRequests <= this->reqProduced)
        {
            this->maxReqHit = true;
            pthread_cond_broadcast(&unconsumedSeats);
            pthread_cond_broadcast(&VipSeatsAvail);
            pthread_cond_broadcast(&seatsAvail);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        if (request == VIPRoom)
        {
            pthread_cond_wait(&VipSeatsAvail, &this->mutex);
        }
        else
        {
            pthread_cond_wait(&seatsAvail, &this->mutex);
        }
    }

    if (this->maxProdRequests <= this->reqProduced)
    {
        this->maxReqHit = true;
        pthread_cond_broadcast(&unconsumedSeats);
        pthread_cond_broadcast(&VipSeatsAvail);
        pthread_cond_broadcast(&seatsAvail);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    onlyItem = (queueGenReq == 0);
    this->buffer.push(request);
    this->reqProduced += 1;
    this->queueGenReq += 1;
    if (request == VIPRoom)
    {
        this->queueVipReq += 1;
    }
    this->prodByType[request] += 1;
    this->queueTypes[request] += 1;
    output_request_added(request, this->prodByType, this->queueTypes);
    if (onlyItem)
    {
        pthread_cond_signal(&this->unconsumedSeats);
    }
    if (this->maxProdRequests <= this->reqProduced)
    {
        this->maxReqHit = true;
        pthread_cond_broadcast(&unconsumedSeats);
        pthread_cond_broadcast(&VipSeatsAvail);
        pthread_cond_broadcast(&seatsAvail);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&this->mutex);
    return 1;
}
int Monitor::remove(Consumers robot)
{
    RequestType request;
    bool atCapacity;
    pthread_mutex_lock(&mutex);
    while (this->queueGenReq == 0)
    {
        // exit if production ended dont wait
        if (maxReqHit)
        {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        pthread_cond_wait(&unconsumedSeats, &this->mutex);
    }
    atCapacity = (queueGenReq == this->normalCapacity);
    request = this->buffer.front();
    this->buffer.pop();
    this->queueGenReq -= 1;
    this->consByRob[robot] += 1;
    this->consByRobType[robot][request] += 1;
    this->queueTypes[request] -= 1;
    if (request == VIPRoom)
    {
        this->queueVipReq -= 1;
    }
    this->consByType[request] += 1;
    output_request_removed(robot, request, this->consByRobType[robot], this->queueTypes);
    if (maxReqHit && this->queueGenReq <= 0 && unlockedBarrier == false)
    {
        unlockedBarrier = true;
        sem_post(this->barrierSem);
        output_production_history(this->prodByType, (unsigned int **)this->consByRobType);
    }
    if (request == VIPRoom)
    {
        pthread_cond_signal(&this->VipSeatsAvail);
    }
    pthread_cond_signal(&this->seatsAvail);
    pthread_mutex_unlock(&this->mutex);
    return 1;
}