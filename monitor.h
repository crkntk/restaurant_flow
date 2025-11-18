#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include "seating.h"
#define MONITOR_GEN_CAP 20 // We define our buffer capacity here
#define MONITOR_VIP_CAP 6  // We define our Vip room capacity here
#ifndef MONITOR_H
#define MONITOR_H
using namespace std;
/*
This is the monitor class that handles all the requests to remove and insert to the buffer
It handles its capancity and signals by condition to other threads running the remove and insert functions
Handles the vip room space and general space as well as signaling the consumer threads for running the critical section and
waiting for more producers to make requests as well as the last consumer and max requests
*/
class Monitor
{
public:
    /* This is a constructor for our monitor object it takes in the maximum amount of requests that can be produced
        and the barrier semaphore to signal from the last consumer for main thread to continue
        Our general capacity and vip capacity are default arguments unless specified in future iterations we may want
        the user to have control over how much space there is for the general buffer capacity and the vip cacpacity
    */
    Monitor(int maxProdReq, sem_t *barrierSem, int genCapacity = MONITOR_GEN_CAP, int vipCapacity = MONITOR_VIP_CAP);
    int insert(RequestType request); // This function tries to insert a request type into the buffer this is based on mutual exclusion and will block if we are out of our contraints for capacity and vip capcity depending on request
    int remove(Consumers robot);     // This function removes a request from the queue it follows mutual exclusion and blocks if there are no items and waits for producer if the max requests has not been hit

private:
    pthread_mutex_t mutex;                                    // This is our mutex for mutually exclusion of critical sections
    pthread_cond_t VipSeatsAvail;                             // This condition is for vip seats that are available to fill
    pthread_cond_t seatsAvail;                                // This is condition to signal or wait on that there are general seats available to fill
    pthread_cond_t unconsumedSeats;                           // This condition is thread condition is to signal or wait on there are seats in buffer that have not been consumed
    sem_t *barrierSem;                                        // This is our pointer to our barrier semaphore to signal our main thread that the last consumer has consumed the last seat so main thread can continue
    unsigned int prodByType[RequestTypeN];                    // This array is to keep track the requests that have been produced by type
    unsigned int consByType[RequestTypeN];                    // This array is to keep track of the requests that have been consumed by type
    unsigned int consByRob[ConsumerTypeN];                    // This array is to keep track of how many requests each type/name of robot have consumed
    unsigned int queueTypes[RequestTypeN];                    // This array keeps track of how many of each type are in the queue
    unsigned int *consByRobType[ConsumerTypeN];               // This is a 2D array the first level is the robot/consumer type and the second level is how many request has that robot/consumer consumed by type
    int maxProdRequests;                                      // This is the max amount of requests that can be produced by producers of any kind
    int normalCapacity;                                       // This is the normal capacity in our buffer
    int VIPCapacity;                                          // This is the vip capacity within our queue capacity. This is not separate from the general capacity it is an amount within the normal queue cacapcity
    int queueGenReq;                                          // How many requests are in the queue in general including vip
    int queueVipReq;                                          // How many vip requests are in teh queue
    int reqProduced;                                          // How many requests have been produced so far
    bool maxReqHit;                                           // The maximum amount of requests producers can produce and insert in the queue overall
    bool unlockedBarrier;                                     // This boolean is to signal that the semaphore barrier has been unlocked that way no more consumers can signal the semaphore after the last consumer has signaled
    queue<RequestType> buffer;                                // This is the general buffer for our requests the Request Type is the type of request that is an enumerator defined in seating.h
    void signal_all_cond(int numConsumers, int numProducers); // This function signals all the conditions currently in our monitor for each thread of consumer and producer
};
#endif