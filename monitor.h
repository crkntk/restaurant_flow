#include <pthread.h>
#include <queue>
#include "seating.h"
#define MONITOR_GEN_CAP 20
#define MONITOR_VIP_CAP 6
#ifndef MONITOR_H
#define MONITOR_H
using namespace std;
class Monitor
{
public:
    Monitor(int maxProdReq, int genCapacity = MONITOR_GEN_CAP, int vipCapacity = MONITOR_VIP_CAP);
    pthread_mutex_t mutex;
    pthread_cond_t VipSeatsAvail;
    pthread_cond_t seatsAvail;
    pthread_cond_t unconsumedSeats;
    int maxProdRequests;
    int normalCapacity;
    int VIPCapacity;
    int queueGenReq;
    int queueVipReq;
    int reqProduced;
    bool maxReqHit;
    queue<RequestType> buffer;
    int insert(RequestType request);
    RequestType remove();
};
#endif