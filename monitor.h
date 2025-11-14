#include <pthread.h>
#include <queue>
#include "seating.h"
#ifndef MONITOR_H
#define MONITOR_H
using namespace std;
class Monitor
{
    Monitor(int genCapacity, int vipCapacity);
    pthread_mutex_t mutex;
    pthread_cond_t VipSeatsAvail;
    pthread_cond_t seatsAvail;
    pthread_cond_t unconsumedSeats;
    int normalCapacity;
    int VIPCapacity;
    int queueGenReq;
    int queueVipReq;
    queue <RequestType> buffer;
    void insert(RequestType request);
    RequestType remove();
};
#endif