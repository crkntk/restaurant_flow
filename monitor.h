#include <pthread.h>
#include <queue>
#include "seating.h"
#ifndef MONITOR_H
#define MONITOR_H

class Monitor
{
    Monitor(int genCapacity, int vipCapacity);
    pthread_mutex_t mutex;
    pthread_cond_t VipSeatsAvail;
    pthread_cond_t seatsAvail;
    int normalCapacity;
    int VIPCapacity;
    int queueGenReq;
    int queueVipReq;
    void insert(RequestType reqType);
    int remove();
};
#endif