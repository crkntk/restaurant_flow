#include "monitor.h"
#include "log.h"
Monitor::Monitor(int maxProdReq, sem_t *barrierSem, int genCapacity, int vipCapacity)
{
    /*
    This contructor is for our monitor to instantiate takes in max produced request allowed for producer threads
    the semaphore barries to signal last consumer the genCapcity which is the general capacity of our queues
    and the vip capacity which is the vip slots within our general capacity
    */
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
    bool vipCapHit = this->queueVipReq >= this->VIPCapacity;
    bool genCapHit = this->queueGenReq >= this->normalCapacity;
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced);
    if (this->maxReqHit == true)
    {
        this->signal_all_cond((int)ConsumerTypeN);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    while ((request == VIPRoom && vipCapHit == true) || genCapHit == true)
    {
        if (this->maxReqHit == true)
        {
            this->signal_all_cond((int)ConsumerTypeN);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        if (genCapHit == true)
        {
            pthread_cond_wait(&seatsAvail, &this->mutex);
        }
        else
        {
            pthread_cond_wait(&VipSeatsAvail, &this->mutex);
        }
        this->maxReqHit = (this->maxProdRequests <= this->reqProduced);
        vipCapHit = this->queueVipReq >= this->VIPCapacity;
        genCapHit = this->queueGenReq >= this->normalCapacity;
    }
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced);
    if (maxReqHit == true)
    {
        this->signal_all_cond((int)ConsumerTypeN);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
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
    pthread_cond_signal(&this->unconsumedSeats);
    this->maxReqHit = this->maxProdRequests <= this->reqProduced;
    if (this->maxReqHit == true)
    {
        this->signal_all_cond((int)ConsumerTypeN);
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
        if (maxReqHit == true)
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
        this->signal_all_cond((int)ConsumerTypeN);
    }
    if (request == VIPRoom)
    {
        pthread_cond_signal(&this->VipSeatsAvail);
    }
    pthread_cond_signal(&this->seatsAvail);
    pthread_cond_signal(&this->seatsAvail);
    pthread_mutex_unlock(&this->mutex);
    return 1;
}

void Monitor::signal_all_cond(int times)
{
    for (int i = 0; i < times; i++)
    {
        pthread_cond_signal(&(this->unconsumedSeats));
        pthread_cond_signal(&(this->VipSeatsAvail));
        pthread_cond_signal(&(this->seatsAvail));
    }
    return;
}