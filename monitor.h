#include <pthread.h>
#include <semaphore.h>
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
    Monitor(int maxProdReq, sem_t *barrierSem, int genCapacity = MONITOR_GEN_CAP, int vipCapacity = MONITOR_VIP_CAP);
    pthread_mutex_t mutex;
    pthread_cond_t VipSeatsAvail;
    pthread_cond_t seatsAvail;
    pthread_cond_t unconsumedSeats;
    sem_t *barrierSem;
    int maxProdRequests;
    int normalCapacity;
    int prodByType[RequestTypeN];
    int consByRob[ConsumerTypeN];
    int VIPCapacity;
    int queueGenReq;
    int queueVipReq;
    int reqProduced;
    bool maxReqHit;
    bool unlockedBarrier;
    queue<RequestType> buffer;
    int insert(RequestType request);
    RequestType *remove();
};
#endif